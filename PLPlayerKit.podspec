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
  s.version          = "2.4.3"
  s.summary          = "Pili iOS video player SDK, RTMP, HLS video streaming supported."
  s.homepage         = "https://github.com/pili-engineering/PLPlayerKit"
  s.license          = 'Apache License, Version 2.0'
  s.author           = { "0dayZh" => "0day.zh@gmail.com" }
  s.source           = { :git => "https://github.com/pili-engineering/PLPlayerKit.git", :tag => "v#{s.version}" }

  s.platform     = :ios, '7.0'
  s.requires_arc = true

  s.public_header_files = "Pod/Library/include/PLPlayerKit/*.h"
  s.source_files = 'Pod/Library/include/**/*.[h|m]'
  s.vendored_libraries   = 'Pod/Library/lib/*.a'

  s.dependency 'HappyDNS', '~> 0.3.10'
  s.frameworks = ["UIKit", "Foundation", "CoreGraphics", "MediaPlayer", "CoreAudio", "AudioToolbox", "Accelerate", "QuartzCore", "OpenGLES", "AVFoundation"]
  s.libraries = "c++", "z", "bz2", "iconv"

end
