//
//  MSOpusEncoder.h
//  media
//
//  Created by ftanx on 2017/6/16.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import <Foundation/Foundation.h>

@interface MSOpusEncoder : NSObject

-(MSOpusEncoder*)init;
-(int)Encode:(uint8_t*)inBuf encodeData:(uint8_t*)outBuf;

@end
