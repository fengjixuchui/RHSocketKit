## RHSocketKit
[![Build Status](https://travis-ci.org/zhu410289616/RHSocketKit.svg?branch=master)](https://travis-ci.org/zhu410289616/RHSocketKit)
[![Version Status](https://img.shields.io/cocoapods/v/RHSocketKit.svg?style=flat)](http://cocoadocs.org/docsets/RHSocketKit)
[![Analytics](https://ga-beacon.appspot.com/UA-78533289-1/welcome-page)](https://github.com/zhu410289616/RHSocketKit)
[![Platform](http://img.shields.io/cocoapods/p/RHSocketKit.svg?style=flat)](http://cocoapods.org/?q=RHSocketKit)
[![License](https://img.shields.io/cocoapods/l/RHSocketKit.svg)](http://cocoadocs.org/docsets/RHSocketKit)

**socket网络通信框架**

虽然CocoaAsyncSocket已经非常的成熟，但是项目，业务，协议等不同导致tcp模块的公用性不高，需要根据协议重新订制调整，不能直接拷贝框架使用。
为了减少调整消耗的时间，对tcp模块中相关的内容进行拆分实现。

### 功能简介
* TCP支持缓存拼包功能
* 支持快速实现自定义编码解码
* 默认支持分隔符编解码、可变长度编解码、protobuf基础通信编解码
* 支持发送数据包内存级缓存
* 支持心跳设置，快速自定义
* 支持TCP自动重连
* TCP链接支持1对多观察者
* 通信底层增加interceptor拦截器，方便数据观察及其他自定义扩展

其中专为和netty通信互通而实现了3个主要编解码器：

* 1 分隔符编解码：RHSocketDelimiterDecoder, RHSocketDelimiterEncoder
* 2 可变长度编解码：RHSocketVariableLengthDecoder, RHSocketVariableLengthEncoder
* 3 protobuf通信编解码：RHProtobufVarint32LengthDecoder, RHProtobufVarint32LengthEncoder


## 工程Demo迁移

<mark>Example工程较久未维护，计划废弃；目前代码已经迁移到Pipeline目录下的Pipeline工程中，想要学习了解的同学可以直接跑Pipeline工程；</mark>

> 进入到Pipeline目录，然后只想 **pod install** 命令
> 
> 如果在Xcode10中出现如下错误提升：
> 
> Multiple commands produce '/Users/zhuruhong/Library/Developer/Xcode/DerivedData/Pipeline-hfoiorbrhlstyohibtuanawzieym/Build/Products/Debug-iphonesimulator/Pipeline.app':
> 
> 1) Target 'Pipeline' has create directory command with output '/Users/zhuruhong/Library/Developer/Xcode/DerivedData/Pipeline-hfoiorbrhlstyohibtuanawzieym/Build/Products/Debug-iphonesimulator/Pipeline.app'
>
> 2) That command depends on command in Target 'Pipeline': script phase "[CP] Copy Pods Resources"
>
> 请按照下面方法解决：
> 
> 1. In Xcode, go to File->Project/Workspace settings.
> 
> 2. Change the build system to Legacy Build system.
> 


## CocoaPods
```
pod 'RHSocketKit'
```


## Features [Log](https://github.com/zhu410289616/RHSocketKit/blob/master/Log.md)
框架的设计运用了一些设计模式，通过组合和协议编程，灵活多变，扩展方便。

**目前框架的核心在core目录中，主要组件有connection，channel，packet，codec，exception，utils，其中codec的实现是业务重点。**

另外，RHSocketService是一个单例实现，方便直接调用。RPC是一个深度封装的内容，需要理解框架才能使用。


![image](./Docs/RHSocketUML.png)


---
### RHSocketConnection
```
socket网络连接对象，只负责socket网络的连接通信，内部使用GCDAsyncSocket。
1-只公开GCDAsyncSocket的主要方法，增加使用的便捷性。
2-封装的另一个目的是，易于后续更新调整。如果不想使用GCDAsyncSocket，只想修改内部实现即可，对外不产生影响。
```

### RHSocketChannel
```
在RHSocketConnection基础上做封装，负责对socket中的二进制通讯数据做缓存、粘包，内部需要编码、解码处理。
1-需要有一个编码解码器，对数据块做封包和解包。很多人不理解这个，其实很简单。比如一句话里面没有标点符号你怎么知道什么时候是结束什么时候开始呢
2-内部带有一个数据缓存，用于对数据的拼包。发送网络数据时，一条数据会被切成多个网络包［不是我们上层协议中的概念］，需要对收到的数据做合并，完整后才能正常解码。
```

### Packet-数据包协议
```
/**
 *  数据包协议
 */
@protocol RHSocketPacket <NSObject>

@required

/**
 *  数据包携带的数据变量（可以是任何数据格式）
 */
@property (nonatomic, strong) id object;

@optional

/**
 *  类似tag，必要的时候实现，用于区分某个数据包
 */
@property (nonatomic, assign) NSInteger pid;

- (instancetype)initWithObject:(id)aObject;

@end
```

```
/**
 *  上行数据包协议，发送数据时，必须遵循的协议
 */
@protocol RHUpstreamPacket <RHSocketPacket>

@optional

/**
 *  发送数据超时时间，必须设置。－1时为无限等待
 */
@property (nonatomic, assign) NSTimeInterval timeout;

@end
```

```
/**
 *  下行数据包协议，接收数据时，必须遵循的协议
 */
@protocol RHDownstreamPacket <RHSocketPacket>

@end
```

### Codec－编码器encoder和解码器decoder
`编码器和解码器是应用传输协议直接相关的重点，简单的有：`

* 分隔符编码器RHSocketDelimiterEncoder和解码器RHSocketDelimiterDecoder。
* 可变长度编码器RHSocketVariableLengthEncoder和解码器RHSocketVariableLengthDecoder。

为了保证数据安全，一般都会在协议上做一些特殊定义，防止被恶意访问，所以自定义协议是必须的。框架的编码和解码协议如下，可以自由实现，满足需求。demo可以查看RHSocketCustomCodecDemo和基于本框架的RHMQTTKit。

```
/**
 *  编码器协议
 */
@protocol RHSocketEncoderProtocol <NSObject>

@required

/**
 *  编码器
 *
 *  @param packet 待发送的数据包
 *  @param output 数据编码后，分发对象
 */
- (void)encode:(id<RHUpstreamPacket>)upstreamPacket output:(id<RHSocketEncoderOutputProtocol>)output;

@end
```

```
/**
 *  解码器协议
 */
@protocol RHSocketDecoderProtocol <NSObject>

/**
 *  解码器
 *
 *  @param downstreamPacket 接收到的原始数据
 *  @param output           数据解码后，分发对象
 *
 *  @return -1解码异常，断开连接; 0数据不完整，等待数据包; >0解码正常，为已解码数据长度
 */
- (NSInteger)decode:(id<RHDownstreamPacket>)downstreamPacket output:(id<RHSocketDecoderOutputProtocol>)output;

@end
```

## Issue

* <mark>[感谢snilwx的反馈]</mark> **数据缓存过大的控制问题**：在RHSocketChannel的didRead:withData:tag:方法中，若网络积压，造成多个数据包拼接，一次性读取到大量数据，超过最大值maxFrameSize设置时，会直接断开链接，会造成当前链接的数据接收没有被正常处理。**缓存接收到的数据大小，由缓存模块做逻辑控制更加合理，解码器只需要关心解码是否正常。若缓存超过缓存区最大值，则可以按照缓存模块的分块逻辑，对整块缓存做分块切割，然后按次丢给解码器处理**

* **上行数据缓存、重发**：发送失败数据重发，包括未连接时发送数据包，发送失败数据包
* **下行数据缓存**：下行数据包缓存，处理拼包、粘包问题
* **数据测试demo**：在app内闭环数据测试逻辑，包括不同的编码器、解码器逻辑
* **SDK被集成统计**：缺少被集成数据统计逻辑，增加简单控制，参考lookin




</br>

---
#### [RHSocketKit网络通信使用之tcp连接（一）](http://blog.csdn.net/zhu410289616/article/details/46731605)
#### [RHSocketKit网络通信使用之数据编码和解码（二）](http://blog.csdn.net/zhu410289616/article/details/46739019)
#### [RHSocketKit网络通信使用之http协议测试（三）](http://blog.csdn.net/zhu410289616/article/details/46746683)
#### [网络通信使用之RHSocketKit框架（四）](http://blog.csdn.net/zhu410289616/article/details/49331323)
#### [RHSocketKit框架简述视频](http://www.tudou.com/programs/view/OahFYRBIFJA/)
#### [RHSocketKit网络通信使用之Protobuf安装（五）](http://blog.csdn.net/zhu410289616/article/details/50739164)
#### [利用RHSocketKit构建自定义协议通信](http://blog.csdn.net/zhu410289616/article/details/51182751)

</br>

## 联系方式
qq:        410289616<br/>
email:     <zhu410289616@163.com><br/>
qq群1:      330585393<br/>
qq群2:      371293816<br/>
qq群3:      231199626<br/>

如果觉得我的源码对您有帮助，请点击`star`和`fork`。</br>
如果您正好有余粮，欢迎随意打赏。</br>
您的奖励是给予我莫大的鼓励和支持！</br>
![image](https://raw.githubusercontent.com/zhu410289616/RHSocketKit/master/Docs/wechat_pay.jpg)
