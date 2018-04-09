# MMMaterialDesignSpinner

[![CI Status](http://img.shields.io/travis/misterwell/MMMaterialDesignSpinner.svg?style=flat)](https://travis-ci.org/misterwell/MMMaterialDesignSpinner)
[![Version](https://img.shields.io/cocoapods/v/MMMaterialDesignSpinner.svg?style=flat)](http://cocoadocs.org/docsets/MMMaterialDesignSpinner)
[![License](https://img.shields.io/cocoapods/l/MMMaterialDesignSpinner.svg?style=flat)](http://cocoadocs.org/docsets/MMMaterialDesignSpinner)
[![Platform](https://img.shields.io/cocoapods/p/MMMaterialDesignSpinner.svg?style=flat)](http://cocoadocs.org/docsets/MMMaterialDesignSpinner)
[![Carthage compatible](https://img.shields.io/badge/Carthage-compatible-4BC51D.svg?style=flat)](https://github.com/Carthage/Carthage)

![Demo](https://raw.githubusercontent.com/misterwell/MMMaterialDesignSpinner/master/Demo.gif "Pod Demo")

## Usage

To run the example project, clone the repo, and run `pod install` from the Example directory first.

``` objc
// Initialize the progress view
MMMaterialDesignSpinner *spinnerView = [[MMMaterialDesignSpinner alloc] initWithFrame:CGRectMake(0, 0, 40, 40)];

// Set the line width of the spinner
spinnerView.lineWidth = 1.5f;
// Set the tint color of the spinner
spinnerView.tintColor = [UIColor redColor];

// Add it as a subview
[self.view addSubview:spinnerView];

...

// Start & stop animations
[spinnerView startAnimating];
[spinnerView stopAnimating];

```
Also Support Xib & StoryBoard


The `lineWidth` and `tintColor` properties can even be set after animating has been started, which you can observe in the included example project.

## Requirements
* CoreGraphics

## Installation

MaterialDesignSpinner is available through [CocoaPods](http://cocoapods.org). To install
it, simply add the following line to your Podfile:

    pod "MMMaterialDesignSpinner"

MaterialDesignSpinner is also [Carthage](https://github.com/Carthage/Carthage) compatible, add the following line to your Cartfile:

    github "misterwell/MMMaterialDesignSpinner"

## Author

Mike Maxwell, mmaxwell@vertical.com

## License

MMMaterialDesignSpinner is available under the MIT license. See the LICENSE file for more info.

