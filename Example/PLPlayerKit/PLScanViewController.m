//
//  PLScanViewController.m
//  PLPlayerKitDemo
//
//  Created by liang on 6/21/16.
//  Copyright © 2016 Pili Engineering, Qiniu Inc. All rights reserved.
//

#import <AVFoundation/AVFoundation.h>
#import "PLScanViewController.h"

@interface PLScanViewController ()<
  AVCaptureMetadataOutputObjectsDelegate,
  UIAlertViewDelegate
>

@property (nonatomic, strong) UIView *boxView;
@property (nonatomic, strong) CALayer *scanLayer;
@property (nonatomic, strong) AVCaptureSession *captureSession;
@property (nonatomic, strong) AVCaptureVideoPreviewLayer *videoPreviewLayer;

@property (nonatomic, strong) NSString *scanResult;

@end

@implementation PLScanViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    [self startReading];
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self stopReading];
}

- (BOOL)startReading {
    NSError *error;
    
    //1.初始化捕捉设备（AVCaptureDevice），类型为AVMediaTypeVideo
    AVCaptureDevice *captureDevice = [AVCaptureDevice defaultDeviceWithMediaType:AVMediaTypeVideo];
    
    //2.用captureDevice创建输入流
    AVCaptureDeviceInput *input = [AVCaptureDeviceInput deviceInputWithDevice:captureDevice error:&error];
    if (!input) {
        NSLog(@"%@", [error localizedDescription]);
        return NO;
    }
    
    //3.创建媒体数据输出流
    AVCaptureMetadataOutput *captureMetadataOutput = [[AVCaptureMetadataOutput alloc] init];
    
    //4.实例化捕捉会话
    _captureSession = [[AVCaptureSession alloc] init];
    
    //4.1.将输入流添加到会话
    [_captureSession addInput:input];
    
    //4.2.将媒体输出流添加到会话中
    [_captureSession addOutput:captureMetadataOutput];
    
    //5.创建串行队列，并加媒体输出流添加到队列当中
    dispatch_queue_t dispatchQueue;
    dispatchQueue = dispatch_queue_create("myQueue", NULL);
    //5.1.设置代理
    [captureMetadataOutput setMetadataObjectsDelegate:self queue:dispatchQueue];
    
    //5.2.设置输出媒体数据类型为QRCode
    [captureMetadataOutput setMetadataObjectTypes:[NSArray arrayWithObject:AVMetadataObjectTypeQRCode]];
    
    //6.实例化预览图层
    _videoPreviewLayer = [[AVCaptureVideoPreviewLayer alloc] initWithSession:_captureSession];
    
    //7.设置预览图层填充方式
    [_videoPreviewLayer setVideoGravity:AVLayerVideoGravityResizeAspectFill];
    
    //8.设置图层的frame
    [_videoPreviewLayer setFrame:self.view.layer.bounds];
    
    //9.将图层添加到预览view的图层上
    [self.view.layer addSublayer:_videoPreviewLayer];
    
    //10.设置扫描范围
    captureMetadataOutput.rectOfInterest = CGRectMake(0.2f, 0.2f, 0.8f, 0.8f);
    
    //10.1.扫描框
    CGSize size = self.view.bounds.size;
    _boxView = [[UIView alloc] initWithFrame:CGRectMake(size.width * 0.2f,
                                                        size.height * 0.2f,
                                                        size.width - size.width * 0.4f,
                                                        size.height - size.height * 0.4f)];
    _boxView.layer.borderColor = [UIColor greenColor].CGColor;
    _boxView.layer.borderWidth = 1.0f;
    
    [self.view addSubview:_boxView];
    
    //10.2.扫描线
    _scanLayer = [[CALayer alloc] init];
    _scanLayer.frame = CGRectMake(0, 0, _boxView.bounds.size.width, 1);
    _scanLayer.backgroundColor = [UIColor brownColor].CGColor;
    
    [_boxView.layer addSublayer:_scanLayer];
    
    NSTimer *timer = [NSTimer scheduledTimerWithTimeInterval:0.2f target:self selector:@selector(moveScanLayer:) userInfo:nil repeats:YES];
    
    [timer fire];
    
    //10.开始扫描
    [_captureSession startRunning];
    
    
    return YES;
}


-(void)stopReading{
    [_captureSession stopRunning];
    _captureSession = nil;
    [_scanLayer removeFromSuperlayer];
    [_videoPreviewLayer removeFromSuperlayer];
}

#pragma mark - UIAlertViewDeletgate
- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if (buttonIndex == 0) {
        [self startReading];
    } else {
        if ([self.delegate respondsToSelector:@selector(scanQRResult:)]) {
            [self.delegate scanQRResult:self.scanResult];
        }
        [self.navigationController popViewControllerAnimated:YES];
    }
}

#pragma mark - AVCaptureMetadataOutputObjectsDelegate
- (void)captureOutput:(AVCaptureOutput *)captureOutput
didOutputMetadataObjects:(NSArray *)metadataObjects
       fromConnection:(AVCaptureConnection *)connection {
    //判断是否有数据
    if (metadataObjects != nil && [metadataObjects count] > 0) {
        AVMetadataMachineReadableCodeObject *metadataObj = [metadataObjects objectAtIndex:0];
        //判断回传的数据类型
        if ([[metadataObj type] isEqualToString:AVMetadataObjectTypeQRCode]) {
            NSLog(@"input QR: %@", [metadataObj stringValue]);
            self.scanResult = [metadataObj stringValue];
            [self performSelectorOnMainThread:@selector(stopReading) withObject:nil waitUntilDone:NO];
            
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:nil
                                                                message:self.scanResult
                                                               delegate:self
                                                      cancelButtonTitle:@"cancel"
                                                      otherButtonTitles:@"OK", nil];
            dispatch_async(dispatch_get_main_queue(), ^{
                [alertView show];
            });
        }
    }
}

- (void)moveScanLayer:(NSTimer *)timer {
    CGRect frame = _scanLayer.frame;
    if (_boxView.frame.size.height < _scanLayer.frame.origin.y) {
        frame.origin.y = 0;
        _scanLayer.frame = frame;
    }else{
        
        frame.origin.y += 5;
        
        [UIView animateWithDuration:0.1 animations:^{
            _scanLayer.frame = frame;
        }];
    }
}

- (BOOL)shouldAutorotate {
    return NO;
}

@end
