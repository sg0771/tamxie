//
//  ViewController.h
//  VA
//
//  Created by TenXie on 2017/2/16.
//  Copyright © 2017年 TenXie. All rights reserved.
//

#import <UIKit/UIKit.h>

//Play your capture!!

#include "WXBase.h"

@interface ViewController : UIViewController{
    __weak IBOutlet UIView *m_va;
    __weak IBOutlet UITextField *m_label;
}
- (IBAction)onStart:(id)sender;
- (IBAction)onStop:(id)sender;

@end

