//
//  ViewController.h
//  WXImageTestMac
//
//  Created by momo on 2021/10/25.
//

#import <Cocoa/Cocoa.h>

@interface ViewController : NSViewController

- (IBAction)onInput:(id)sender;
- (IBAction)onOutput:(id)sender;


@property (weak) IBOutlet NSTextField *m_txtInput;
@property (weak) IBOutlet NSTextField *m_txtOutput;
@property (weak) IBOutlet NSTextField *m_log;
@property (weak) IBOutlet NSTextField *m_txtQuality;
@property (weak) IBOutlet NSTextField *m_txtDstWidth;
@property (weak) IBOutlet NSTextField *m_txtDstHeight;


- (IBAction)onQualityOrig:(id)sender;
- (IBAction)onQualityJpeg:(id)sender;
- (IBAction)onQualityPng:(id)sender;
- (IBAction)onQualityWebp:(id)sender;
- (IBAction)onQualityPng8:(id)sender;


@property (weak) IBOutlet NSTextField *m_txtSize;
- (IBAction)onSizeOrig:(id)sender;
- (IBAction)onSizeJpg:(id)sender;
- (IBAction)onSizeWebp:(id)sender;



@end

