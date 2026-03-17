%ifdef FFMPEG_ARM64
	%include "config.arm64.asm"
%endif
%ifdef FFMPEG_ARMV7
	%include "config.armv7.asm"
%endif
%ifdef FFMPEG_ARMV5
	%include "config.armv5.asm"
%endif
%ifdef FFMPEG_X64
	%include "config.x64.asm"
%endif
%ifdef FFMPEG_X86
	%include "config.x86.asm"
%endif
