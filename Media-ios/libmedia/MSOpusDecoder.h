//
//  MSOpusDecoder.h
//  media
//
//  Created by ftanx on 2017/6/16.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MSOpusDecoder : NSObject

-(MSOpusDecoder*)init;
-(int)Decode:(uint8_t*)inBuf decodeData:(uint8_t*)outBuf;

@end
