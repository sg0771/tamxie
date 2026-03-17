//
//  MSVideoCapture.h
//  media
//
//  Created by ftanx on 2017/5/19.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import <Foundation/Foundation.h>
#import "wh_utils_objc.h"

@interface MSVideoCapture : NSObject;

@property  id<DataDelegate> delegate;
@property (atomic, assign)  int   m_vsize;
@property (atomic, assign)  int   m_rotate;
@property (atomic, assign)  int   m_iFps;
@property (atomic, assign)  BOOL  m_isFront;
@property (readonly, assign) BOOL m_isRunning;
@property (readonly, assign) int  m_iWidth;
@property (readonly, assign) int  m_iHeight;

- (BOOL) Start;
- (BOOL) Stop;
- (BOOL) Switch;

@end
