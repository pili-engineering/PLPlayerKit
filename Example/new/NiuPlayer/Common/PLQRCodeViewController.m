//
//  PLQRCodeViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/9.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLQRCodeViewController.h"

@interface PLQRCodeViewController ()
<
AVCaptureMetadataOutputObjectsDelegate,
UIImagePickerControllerDelegate,
UINavigationControllerDelegate,
CAAnimationDelegate
>

@property (strong, nonatomic) AVCaptureDevice     *device;
@property (strong, nonatomic) AVCaptureDeviceInput *input;
@property (strong, nonatomic) AVCaptureMetadataOutput *output;
@property (strong, nonatomic) AVCaptureSession *session;
@property (strong, nonatomic) AVCaptureVideoPreviewLayer *previewLayer;

@property (assign, nonatomic) CGRect scanFrame;

@property (nonatomic, strong) UIView *lineView;

@end

@implementation PLQRCodeViewController

- (void)dealloc {
    [self stop];
}


- (void)viewDidLoad {
    
    [super viewDidLoad];
    self.title = @"二维码扫描";
    
    [NSObject haveCameraAccess:^(BOOL isAuth) {
        if (!isAuth) return;
        
        UIBarButtonItem *albumItem = [[UIBarButtonItem alloc] initWithTitle:@"相册" style:(UIBarButtonItemStylePlain) target:self action:@selector(clickAlbumItem)];
        self.navigationItem.rightBarButtonItem = albumItem;
        
        CGFloat scanWidth = 220;
        self.scanFrame = CGRectMake((self.view.bounds.size.width - scanWidth) / 2, (self.view.bounds.size.height - scanWidth) / 2, scanWidth, scanWidth);
        self.lineView = [[UIView alloc] init];
        self.lineView.frame = CGRectMake(self.scanFrame.origin.x + 10, self.scanFrame.origin.y, self.scanFrame.size.width - 10 *2, 2);
        self.lineView.backgroundColor = [UIColor colorWithRed:.2 green:.8 blue:.2 alpha:1];
        [self.view addSubview:self.lineView];
        self.lineView.hidden = YES;
        
        UILabel *alertLabel = [[UILabel alloc] init];
        alertLabel.font = [UIFont systemFontOfSize:12];
        alertLabel.text = @"将二维码放入框内，即可自动扫描";
        alertLabel.textColor = [UIColor whiteColor];
        alertLabel.textAlignment = NSTextAlignmentCenter;
        
        [self.view addSubview:alertLabel];
        [alertLabel mas_makeConstraints:^(MASConstraintMaker *make) {
            make.centerX.equalTo(self.view);
            make.top.equalTo(self.scanFrame.origin.y + self.scanFrame.size.height + 5);
        }];
        
        [self setCropRect:self.scanFrame];
        
        [self setupSession];
        [self start];
    }];
}

- (void)setCropRect:(CGRect)cropRect{
    
    CAShapeLayer *cropLayer = [[CAShapeLayer alloc] init];
    CGMutablePathRef path = CGPathCreateMutable();
    CGPathAddRect(path, nil, cropRect);
    CGPathAddRect(path, nil, self.view.bounds);
    
    [cropLayer setFillRule:kCAFillRuleEvenOdd];
    [cropLayer setPath:path];
    [cropLayer setFillColor:[UIColor blackColor].CGColor];
    [cropLayer setOpacity:0.5];
    
    [self.view.layer addSublayer:cropLayer];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (BOOL)hidesBottomBarWhenPushed {
    return YES;
}

-(void)viewDidAppear:(BOOL)animated {
    [super viewDidAppear:animated];
    
    [self start];
}

- (void)viewWillDisappear:(BOOL)animated {
    [super viewWillDisappear:animated];
    [NSObject cancelPreviousPerformRequestsWithTarget:self selector:@selector(start) object:nil];
    [self stop];
}

- (void)start {
    if (self.session && ![self.session isRunning]) {
        [self.session startRunning];
        [self animateLine];
    }
}

- (void)stop {
    [self.session stopRunning];
    [self.lineView.layer removeAllAnimations];
    self.lineView.hidden = YES;
}

- (void)animateLine {
    
    if (![self.session isRunning]) return;
    
    self.lineView.hidden = NO;
    [self.lineView.layer removeAllAnimations];
    
    CABasicAnimation *animation = [CABasicAnimation animationWithKeyPath:@"position"];
    CGPoint fromPoint = CGPointMake(self.scanFrame.origin.x + self.scanFrame.size.width / 2, self.scanFrame.origin.y);
    CGPoint toPoint = CGPointMake(self.scanFrame.origin.x + self.scanFrame.size.width / 2, self.scanFrame.origin.y + self.scanFrame.size.height);
    animation.fromValue = [NSValue valueWithCGPoint:fromPoint];
    animation.toValue   = [NSValue valueWithCGPoint:toPoint];
    animation.duration  = 2;
    animation.beginTime = 0;
    animation.delegate  = self;
    
    [self.lineView.layer addAnimation:animation forKey:@"down"];
}

- (void)animationDidStop:(CAAnimation *)anim finished:(BOOL)flag {
    if (flag) {
        [self animateLine];
    }
}

- (void)setupSession
{
    BOOL result = NO;
    
    do {
        NSArray *devices = [AVCaptureDevice devicesWithMediaType:AVMediaTypeVideo];
        for (AVCaptureDevice *device in devices) {
            if (AVCaptureDevicePositionBack == [device position]) {
                self.device = device;
                break;
            }
        }
        
        if (!self.device) break;
        
        self.input = [AVCaptureDeviceInput deviceInputWithDevice:self.device error:nil];
        
        self.output = [[AVCaptureMetadataOutput alloc]init];
        [self.output setMetadataObjectsDelegate:self queue:dispatch_get_main_queue()];
        
        [self.output setRectOfInterest:CGRectMake(self.scanFrame.origin.y / self.view.bounds.size.height, self.scanFrame.origin.x / self.view.bounds.size.width, self.scanFrame.size.height / self.view.bounds.size.height, self.scanFrame.size.width / self.view.bounds.size.width)];
        
        self.session = [[AVCaptureSession alloc]init];
        [self.session setSessionPreset:AVCaptureSessionPresetHigh];
        if ([self.session canAddInput:self.input]) {
            [self.session addInput:self.input];
        } else {
            break;
        }
        
        if ([self.session canAddOutput:self.output]) {
            [self.session addOutput:self.output];
        } else {
            break;
        }
        [self.output setMetadataObjectTypes:@[AVMetadataObjectTypeQRCode]];

        self.previewLayer = [AVCaptureVideoPreviewLayer layerWithSession:self.session];
        self.previewLayer.videoGravity = AVLayerVideoGravityResizeAspectFill;
        self.previewLayer.frame = self.view.bounds;
        [self.view.layer insertSublayer:self.previewLayer atIndex:0];
        
        result = YES;
    } while (0);
    
    if (!result) {
        [self.view showTip:@"打开摄像头失败"];
    }
}

- (void)captureOutput:(AVCaptureOutput *)captureOutput didOutputMetadataObjects:(NSArray *)metadataObjects fromConnection:(AVCaptureConnection *)connection {
    if ([metadataObjects count] >0) {
        //停止扫描
        [self stop];

        AVMetadataMachineReadableCodeObject * metadataObject = [metadataObjects firstObject];
        NSString * stringValue = metadataObject.stringValue;
        NSLog(@"识别二维码结果: %@", stringValue);
        if ([stringValue isURL]) {
            [self.navigationController popViewControllerAnimated:YES];
            [self.delegate codeViewController:self scanResult:stringValue];
        } else {
            [self.view showTip:@"扫描失败，重新扫描"];
            [self performSelector:@selector(start) withObject:nil afterDelay:2];
        }
    }
}


- (void)clickAlbumItem {
    
    [NSObject haveAlbumAccess:^(BOOL isAuth) {
        if (!isAuth) return;
      
        UIImagePickerController *imagePicker = [[UIImagePickerController alloc] init];
        imagePicker.editing = NO;
        imagePicker.delegate = self;
        
        [self presentViewController:imagePicker animated:YES completion:nil];
    }];
}


- (void)imagePickerController:(UIImagePickerController *)picker didFinishPickingMediaWithInfo:(NSDictionary<NSString *,id> *)info {
    
    [self dismissViewControllerAnimated:YES completion:nil];

    UIImage *image = [info objectForKey:UIImagePickerControllerOriginalImage];
    NSString * stringValue = [self stringFromImageQRCode:image];
    if (0 == stringValue.length) {
        [self.view showTip:@"没有识别到任何信息"];
        return;
    }
    if ([stringValue isURL]) {
        [self.navigationController popViewControllerAnimated:YES];
        [self.delegate codeViewController:self scanResult:stringValue];
    } else {
        [self.view showTip:[NSString stringWithFormat:@"识别结果:%@", stringValue]];
    }
}

- (void)imagePickerControllerDidCancel:(UIImagePickerController *)picker {
    [self dismissViewControllerAnimated:YES completion:nil];
}

- (NSString *)stringFromImageQRCode:(UIImage *)image {
    
    if (!image) return nil;
    
    CIDetector *detector = [CIDetector detectorOfType:CIDetectorTypeQRCode
                                              context:[CIContext contextWithOptions:nil]
                                              options:@{CIDetectorAccuracy : CIDetectorAccuracyHigh}];
    CIImage *ciImage = [image CIImage];
    if (!ciImage) {
        ciImage = [CIImage imageWithCGImage:image.CGImage];
    }
    NSArray * resultArray = [detector featuresInImage:ciImage];
    if (resultArray.count) {
        CIFeature *feature = resultArray[0];
        CIQRCodeFeature *qrFeature = (CIQRCodeFeature *)feature;
        NSString *result = qrFeature.messageString;
        NSLog(@"识别照片结果: %@", result);
        return result;
    }
    return nil;
}

@end
