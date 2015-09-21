//
//  MasterViewController.m
//  PLPlayerKitDemo
//
//  Created by 0day on 14/11/13.
//  Copyright (c) 2014年 qgenius. All rights reserved.
//

#import "MasterViewController.h"
#import <PLPlayerKit/PLPlayerKit.h>
#import "VideoPlayerViewController.h"
#import "AudioPlayerViewController.h"

#warning 更改为你的播放地址
#define PLAY_URL    @"rtmp://fcx0xh.live1-rtmp.z1.pili.qiniucdn.com/dayzh_staging/test"

@interface MasterViewController ()
<
UIAlertViewDelegate
>

@property NSMutableArray *objects;
@end

@implementation MasterViewController

- (void)awakeFromNib {
    [super awakeFromNib];
}

- (void)viewDidLoad {
    [super viewDidLoad];
    // Do any additional setup after loading the view, typically from a nib.
    self.navigationItem.leftBarButtonItem = self.editButtonItem;

    UIBarButtonItem *addButton = [[UIBarButtonItem alloc] initWithBarButtonSystemItem:UIBarButtonSystemItemAdd target:self action:@selector(insertNewObject:)];
    self.navigationItem.rightBarButtonItem = addButton;
    
    self.objects = [@[@"http://hlstime2.plu.cn/longzhu/55f24ae4fb16df6181000060.m3u8?start=1442543867&end=1442544826",
                      @"http://hlstime2.plu.cn/longzhu/55f24ae4fb16df6181000060.m3u8?start=1442542332&end=1442542454",
                      @"http://hlstime2.plu.cn/longzhu/55f24ae4fb16df6181000060.m3u8?start=1442541742&end=1442541837"] mutableCopy];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)insertNewObject:(id)sender {
    if (!self.objects) {
        self.objects = [[NSMutableArray alloc] init];
    }
    
    UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"输入待播放地址"
                                                        message:nil
                                                       delegate:self
                                              cancelButtonTitle:@"取消"
                                              otherButtonTitles:@"确定", nil];
    alertView.alertViewStyle = UIAlertViewStylePlainTextInput;
    [alertView show];
}

#pragma mark - UIAlertViewDelegate

- (void)alertView:(UIAlertView *)alertView clickedButtonAtIndex:(NSInteger)buttonIndex {
    if (buttonIndex == alertView.firstOtherButtonIndex) {
        UITextField *textField = [alertView textFieldAtIndex:0];
        NSString *urlString = textField.text;
        if (urlString.length > 0) {
            [self.objects insertObject:urlString atIndex:0];
            NSIndexPath *indexPath = [NSIndexPath indexPathForRow:0 inSection:0];
            [self.tableView insertRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationAutomatic];
        } else {
            UIAlertView *alertView = [[UIAlertView alloc] initWithTitle:@"网址为空"
                                                                message:@"添加失败"
                                                               delegate:nil
                                                      cancelButtonTitle:@"OK"
                                                      otherButtonTitles:nil];
            [alertView show];
        }
    }
}

#pragma mark - Table View

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    [tableView deselectRowAtIndexPath:indexPath animated:YES];
    
    NSString *path;
    NSMutableDictionary *parameters = [NSMutableDictionary dictionary];
    
    path = self.objects[indexPath.row];
    
    // 在 iPhone 端禁用逐行扫描
    if (UI_USER_INTERFACE_IDIOM() == UIUserInterfaceIdiomPhone) {
        parameters[PLVideoParameterDisableDeinterlacing] = @(YES);
    }
    
    // 取消缓存
    parameters[PLPlayerParameterMinBufferedDuration] = @(0.0f);
    parameters[PLPlayerParameterMaxBufferedDuration] = @(0.0f);
    
    // 开启自动播放
    parameters[PLPlayerParameterAutoPlayEnable] = @(YES);
    
    NSURL *url = [NSURL URLWithString:path];
    
    // 使用自定义控件
    VideoPlayerViewController *vc = [[VideoPlayerViewController alloc] initWithURL:url parameters:parameters];
    [self.navigationController pushViewController:vc animated:YES];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    return 1;
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.objects.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    UITableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:@"Cell" forIndexPath:indexPath];

    NSString *urlString = self.objects[indexPath.row];
    cell.textLabel.text = urlString;
    return cell;
}

- (BOOL)tableView:(UITableView *)tableView canEditRowAtIndexPath:(NSIndexPath *)indexPath {
    // Return NO if you do not want the specified item to be editable.
    return YES;
}

- (void)tableView:(UITableView *)tableView commitEditingStyle:(UITableViewCellEditingStyle)editingStyle forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (editingStyle == UITableViewCellEditingStyleDelete) {
        [self.objects removeObjectAtIndex:indexPath.row];
        [tableView deleteRowsAtIndexPaths:@[indexPath] withRowAnimation:UITableViewRowAnimationFade];
    } else if (editingStyle == UITableViewCellEditingStyleInsert) {
        // Create a new instance of the appropriate class, insert it into the array, and add a new row to the table view.
    }
}

@end
