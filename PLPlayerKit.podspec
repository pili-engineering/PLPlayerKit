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
  s.version          = "3.4.1"
  s.summary          = "Pili iOS video player SDK, RTMP, HLS video streaming supported."
  s.homepage         = "https://github.com/pili-engineering/PLPlayerKit"
  s.license          = 'Apache License, Version 2.0'
  s.author           = { "pili" => "pili-coresdk@qiniu.com" }
  s.source           = { :http => "https://sdk-release.qnsdk.com/PLPlayerKit-iphoneos-v3.4.1.zip" }

  s.platform     = :ios, '8.0'
  s.requires_arc = true

  s.subspec "iphoneos" do |ss1|
    ss1.vendored_framework = "Pod/Library/PLPlayerKit.framework"
  end

  s.frameworks = ["UIKit", "Foundation", "MediaPlayer", "CoreAudio", "AudioToolbox", "Accelerate", "QuartzCore", "OpenGLES", "AVFoundation","CoreVideo","AVKit","CoreMedia","VideoToolbox","CoreTelephony"]
  s.libraries = "c++", "z", "bz2", "iconv", "resolv"

end
