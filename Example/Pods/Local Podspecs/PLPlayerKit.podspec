#
# Be sure to run `pod lib lint PLPlayerKit.podspec' to ensure this is a
# valid spec and remove all comments before submitting the spec.
#
# Any lines starting with a # are optional, but encouraged
#
# To learn more about a Podspec see http://guides.cocoapods.org/syntax/podspec.html
#

Pod::Spec.new do |s|
  s.name             = "PLPlayerKit"
  s.version          = "1.1.2"
  s.summary          = "Pili iOS video player SDK, RTMP, HLS video streaming supported."
  s.homepage         = "https://github.com/pili-io/PLPlayerKit"
  s.license          = 'Apache License, Version 2.0'
  s.author           = { "0dayZh" => "0day.zh@gmail.com" }
  s.source           = { :git => "https://github.com/pili-io/PLPlayerKit.git", :tag => s.version.to_s }

  s.platform     = :ios, '6.0'
  s.requires_arc = true

  s.public_header_files = 'Pod/Library/include/**/*.h'
  s.source_files = 'Pod/Library/include/**/*.h'
  s.resource_bundles = {
    'PLPlayerKit' => ['Pod/Assets/images.bundle/*.png']
  }

  s.frameworks = "UIKit", "Foundation", "CoreGraphics", "MediaPlayer", "CoreAudio", "AudioToolbox", "Accelerate", "QuartzCore", "OpenGLES"
  s.libraries = "iconv", "z"

  s.default_subspec = "precompiled"

  s.subspec "precompiled" do |ss|
    ss.preserve_paths         = "Pod/Library/include/**/*.h", 'Pod/Library/lib/*.a'
    ss.vendored_libraries   = 'Pod/Library/lib/arm64/libavcodec-arm64.a', 'Pod/Library/lib/arm64/libavformat-arm64.a', 'Pod/Library/lib/arm64/libavutil-arm64.a', 'Pod/Library/lib/arm64/libswresample-arm64.a', 'Pod/Library/lib/arm64/libswscale-arm64.a', 'Pod/Library/lib/arm64/libavcodec-arm64.a', 'Pod/Library/lib/arm64/libPLPlayerKit-arm64.a', 'Pod/Library/lib/armv7/libavcodec-armv7.a', 'Pod/Library/lib/armv7/libavformat-armv7.a', 'Pod/Library/lib/armv7/libavutil-armv7.a', 'Pod/Library/lib/armv7/libswresample-armv7.a', 'Pod/Library/lib/armv7/libswscale-armv7.a', 'Pod/Library/lib/armv7/libavcodec-armv7.a', 'Pod/Library/lib/armv7/libPLPlayerKit-armv7.a', 'Pod/Library/lib/i386/libavcodec-i386.a', 'Pod/Library/lib/i386/libavformat-i386.a', 'Pod/Library/lib/i386/libavutil-i386.a', 'Pod/Library/lib/i386/libswresample-i386.a', 'Pod/Library/lib/i386/libswscale-i386.a', 'Pod/Library/lib/i386/libavcodec-i386.a', 'Pod/Library/lib/i386/libPLPlayerKit-i386.a', 'Pod/Library/lib/x86_64/libavcodec-x86_64.a', 'Pod/Library/lib/x86_64/libavformat-x86_64.a', 'Pod/Library/lib/x86_64/libavutil-x86_64.a', 'Pod/Library/lib/x86_64/libswresample-x86_64.a', 'Pod/Library/lib/x86_64/libswscale-x86_64.a', 'Pod/Library/lib/x86_64/libavcodec-x86_64.a', 'Pod/Library/lib/x86_64/libPLPlayerKit-x86_64.a'
    ss.xcconfig = { 'HEADER_SEARCH_PATHS' => "${PODS_ROOT}/#{s.name}/PLPlayerKit/lib/include" }
  end
end
