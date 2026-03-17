//
//  ViewController.h
//  MCTest
//
//  Created by momo on 2022/3/23.
//

#import <UIKit/UIKit.h>

#include "MediaConvert.h"

#include "WXFfmpegConvertAPI.h" //播放器API
#include "WaterMarkRemoveAPI.h" //去水印API

@interface ViewController : UIViewController

@property (weak, nonatomic) IBOutlet UILabel *m_label;


@property (weak, nonatomic) IBOutlet UIView *m_view;


- (IBAction)onMC0:(id)sender;

- (IBAction)onMC1:(id)sender;

- (IBAction)onMC2:(id)sender;



@end

