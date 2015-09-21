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
  s.version          = "1.2.19"
  s.summary          = "Pili iOS video player SDK, RTMP, HLS video streaming supported."
  s.homepage         = "https://github.com/pili-engineering/PLPlayerKit"
  s.license          = 'Apache License, Version 2.0'
  s.author           = { "0dayZh" => "0day.zh@gmail.com" }
  s.source           = { :git => "https://github.com/pili-engineering/PLPlayerKit.git", :tag => s.version.to_s }

  s.platform     = :ios, '6.0'
  s.requires_arc = true

  s.public_header_files = "Pod/Library/include/kxmovie/*.h", "Pod/Library/include/PLPlayerKit/*.h"
  s.source_files = 'Pod/Library/include/**/*.h'
  s.resource_bundles = {
    'PLPlayerKit' => ['Pod/Assets/images.bundle/*.png']
  }

  s.frameworks = "UIKit", "Foundation", "CoreGraphics", "MediaPlayer", "CoreAudio", "AudioToolbox", "Accelerate", "QuartzCore", "OpenGLES", "AVFoundation", "CoreMedia"
  s.libraries = "iconv", "z", "bz2"

  s.default_subspec = "precompiled"

  s.subspec "precompiled" do |ss|
    ss.preserve_paths         = "Pod/Library/include/kxmovie/*.h", "Pod/Library/include/PLPlayerKit/*.h", 'Pod/Library/lib/*.a'
    ss.vendored_libraries   = 'Pod/Library/lib/*.a'
    ss.xcconfig = { 'HEADER_SEARCH_PATHS' => "${PODS_ROOT}/#{s.name}/PLPlayerKit/lib/include" }
  end
end
