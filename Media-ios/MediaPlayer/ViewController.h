//
//  ViewController.h
//  Media
//
//  Created by momo on 2018/11/29.
//  Copyright © 2018年 Tam. All rights reserved.
//

#import <Cocoa/Cocoa.h>


#import <CoreAudio/CoreAudio.h>

@interface ViewController : NSViewController

- (IBAction)onSelectFile:(id)sender;
- (IBAction)onPlay:(id)sender;
@property (weak) IBOutlet NSButton *m_btnPlay;

@property (assign)    NSString *m_strInput;
@property (weak) IBOutlet NSTextField *m_textFileName;

@property (weak) IBOutlet NSOpenGLView *m_viewDisplay;

@end

