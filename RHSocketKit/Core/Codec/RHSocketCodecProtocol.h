//
//  RHSocketCodecProtocol.h
//  RHSocketKitDemo
//
//  Created by zhuruhong on 15/12/16.
//  Copyright © 2015年 zhuruhong. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "RHSocketPacketContext.h"

#pragma mark - encoder output protocol

/**
 *  数据编码后，分发对象协议
 */
@protocol RHSocketEncoderOutputProtocol <NSObject>

@required

- (void)didEncode:(NSData *)encodedData timeout:(NSTimeInterval)timeout;

@end

#pragma mark - decoder output protocol

/**
 *  数据解码后，分发对象协议
 */
@protocol RHSocketDecoderOutputProtocol <NSObject>

@required

- (void)didDecode:(id<RHDownstreamPacket>)decodedPacket;

@end

#pragma mark - encoder protocol

/**
 *  编码器协议
 */
@protocol RHSocketEncoderProtocol <NSObject>

@optional

/** 链式编码 */
@property (nonatomic, strong) id<RHSocketEncoderProtocol> nextEncoder;

@required

/**
 *  编码器
 *
 *  @param upstreamPacket   待发送的数据包
 *  @param output           数据编码后，分发对象
 */
- (void)encode:(id<RHUpstreamPacket>)upstreamPacket output:(id<RHSocketEncoderOutputProtocol>)output;

@end

#pragma mark - decoder protocol

/**
 *  解码器协议
 *  先校验decodeData:output:方法，如果未实现，则走decode:output:方法
 */
@protocol RHSocketDecoderProtocol <NSObject>

@optional

/**
 *  解码器 2.3.0
 *
 *  @param downstreamData   接收到的原始数据
 *  @param output           数据解码后，分发对象
 *
 *  @return -1解码异常; 0数据不完整，等待数据包; >0解码正常，为已解码数据长度
 */
- (NSInteger)decodeData:(NSData *)downstreamData output:(id<RHSocketDecoderOutputProtocol>)output;

/** 链式解码 */
@property (nonatomic, strong) id<RHSocketDecoderProtocol> nextDecoder;

@required

/**
 *  解码器
 *
 *  @param downstreamPacket 接收到的原始数据
 *  @param output           数据解码后，分发对象
 *
 *  @return -1解码异常，断开连接; 0数据不完整，等待数据包; >0解码正常，为已解码数据长度
 */
- (NSInteger)decode:(id<RHDownstreamPacket>)downstreamPacket output:(id<RHSocketDecoderOutputProtocol>)output;//准备废弃 DEPRECATED_ATTRIBUTE

@end

#pragma mark -

