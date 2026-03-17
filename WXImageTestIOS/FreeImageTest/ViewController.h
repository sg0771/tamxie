//
//  ViewController.h
//  FreeImageTest
//
//  Created by momo on 2021/10/25.
//

#import <UIKit/UIKit.h>

@interface ViewController : UIViewController

@property (weak, nonatomic) IBOutlet UITextField *m_txtDstWidth;
@property (weak, nonatomic) IBOutlet UITextField *m_txtDstHeight;
@property (weak, nonatomic) IBOutlet UITextField *m_txtQuality;
@property (weak, nonatomic) IBOutlet UITextField *m_txtSize;
@property (weak, nonatomic) IBOutlet UILabel *m_labelLogInfo;


- (IBAction)onQualityOrig:(id)sender;
- (IBAction)onQualityJpeg:(id)sender;
- (IBAction)onQualityPng:(id)sender;
- (IBAction)onQualityWebp:(id)sender;
- (IBAction)onQualityPng8:(id)sender;



- (IBAction)onSizeOrig:(id)sender;
- (IBAction)onSizeJpeg:(id)sender;
- (IBAction)onSizeWebp:(id)sender;



@end

