//
//  AppDelegate.h
//  OSX_Test
//
//  Created by momo on 2023/12/30.
//

#import <Cocoa/Cocoa.h>

@interface AppDelegate : NSObject <NSApplicationDelegate>

- (IBAction)onInputFile:(id)sender;
- (IBAction)onOutputFile:(id)sender;
- (IBAction)onPlay:(id)sender;
- (IBAction)onStop:(id)sender;
- (IBAction)onConvert:(id)sender;

@property (weak) IBOutlet NSTextField *m_txtInput;
@property (weak) IBOutlet NSTextField *m_txtOutput;
@property (weak) IBOutlet NSView *m_image;


@end

