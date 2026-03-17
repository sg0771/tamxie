//
//  ViewController.h
//  MacPlayer
//
//  Created by tam on 2019/12/11.
//  Copyright © 2019年 xkt.space. All rights reserved.
//

#import <Cocoa/Cocoa.h>



@interface ViewController : NSViewController

- (IBAction)onSelectFile:(id)sender; //File
- (IBAction)onPlayFile:(id)sender; //Play Stop Event

@property (weak) IBOutlet NSButton *m_btnPlay; //Play Stop Button
@property (assign)    NSString *m_strInput; //File Name
@property (weak) IBOutlet NSTextField *m_textFileName; //File Name for show
@property (weak) IBOutlet NSView *m_view; //Image View for show

@end

