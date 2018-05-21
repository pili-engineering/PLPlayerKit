//
//  PLLiveViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLLiveViewController.h"
#import "PLLiveTableViewCell.h"

@interface PLLiveViewController ()
<
UITableViewDelegate,
UITableViewDataSource,
PLCodeViewControllerDelegate
>

@property (nonatomic, strong) UITableView *tableView;

@property (nonatomic, strong) NSArray *mediaArray;

@property (nonatomic, strong) UILabel *alertLabel;

@end

@implementation PLLiveViewController

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.title = @"直播";
    
    self.mediaArray = [[NSMutableArray alloc] init];
    
    self.tableView = [[UITableView alloc] initWithFrame:self.view.bounds style:(UITableViewStyleGrouped)];
    self.tableView.delegate = self;
    self.tableView.dataSource = self;
    self.tableView.separatorColor = [UIColor clearColor];
    self.tableView.backgroundColor = [UIColor clearColor];
    
    [self.view addSubview:self.tableView];
    
    [self.tableView mas_makeConstraints:^(MASConstraintMaker *make) {
        make.edges.equalTo(self.view);
    }];
    
    UIBarButtonItem *scanItem = [[UIBarButtonItem alloc] initWithImage:[[UIImage imageNamed:@"scan"] imageWithRenderingMode:(UIImageRenderingModeAlwaysOriginal)] style:(UIBarButtonItemStylePlain) target:self action:@selector(clickScanItem)];
    self.navigationItem.leftBarButtonItem = scanItem;
    
    UIBarButtonItem *urlItem = [[UIBarButtonItem alloc] initWithImage:[[UIImage imageNamed:@"url"] imageWithRenderingMode:(UIImageRenderingModeAlwaysOriginal)] style:(UIBarButtonItemStylePlain) target:self action:@selector(clickURLItem)];
    self.navigationItem.rightBarButtonItem = urlItem;
    
    [self reloadPlayList];
    
    self.reloadButton.hidden = YES;
    self.alertLabel = [[UILabel alloc] init];
    self.alertLabel.font = [UIFont systemFontOfSize:12];
    self.alertLabel.textColor = [UIColor grayColor];
    self.alertLabel.textAlignment = NSTextAlignmentCenter;
    self.alertLabel.numberOfLines = 0;
    self.alertLabel.text = @"暂无直播列表，您可以点击左上角的按钮扫描二维码观看直播、或者点击右上角手动输入直播地址播放";
    [self.view addSubview:self.alertLabel];
    [self.alertLabel mas_makeConstraints:^(MASConstraintMaker *make) {
        make.center.equalTo(self.view);
        make.width.equalTo(self.view).offset(-50);
    }];
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}


- (void)clickScanItem {
    PLQRCodeViewController *scanController = [[PLQRCodeViewController alloc] init];
    scanController.delegate = self;
    [self.navigationController pushViewController:scanController animated:YES];
}

- (void)clickURLItem {
    
    UIAlertController *alert = [UIAlertController alertControllerWithTitle:@"输入URL播放" message:nil preferredStyle:(UIAlertControllerStyleAlert)];
    [alert addTextFieldWithConfigurationHandler:^(UITextField * _Nonnull textField) {
        textField.placeholder = @"输入URL";
    }];
    
    UIAlertAction *cancelAction = [UIAlertAction actionWithTitle:@"取消" style:(UIAlertActionStyleCancel) handler:^(UIAlertAction * _Nonnull action) {
        
    }];
    UIAlertAction *sureAction = [UIAlertAction actionWithTitle:@"确定" style:(UIAlertActionStyleDefault) handler:^(UIAlertAction * _Nonnull action) {
        NSString *urlString = [alert.textFields firstObject].text;
        if ([urlString isURL]) {
            PLPlayViewController *playController = [[PLPlayViewController alloc] init];
            playController.url = [NSURL URLWithString:urlString];
            [self presentViewController:playController animated:YES completion:nil];
        } else {
            if (urlString.length) {
                [self.view showTip:[NSString stringWithFormat:@"%@ 不是一个合法的 URL", urlString]];
            }
        }
    }];
    
    [alert addAction:cancelAction];
    [alert addAction:sureAction];
    [self presentViewController:alert animated:YES completion:nil];
}

- (void)clickReloadButton {
    [self hideReloadButton];
    [self reloadPlayList];
}

- (void)reloadPlayList {
    
    __weak typeof(self) wself = self;
    
    [self.view showLoadingHUD];
    [PLHttpSession requestLiveMediaList:^(NSArray *list, NSError *error) {
        [wself.view hideLoadingHUD];
        wself.mediaArray = list;
        [wself.tableView reloadData];
        
        if (0 == wself.mediaArray.count && error) {
            [wself.view showTip:[NSString stringWithFormat:@"%@", error.description]];
        }
    }];
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    if (self.mediaArray.count) {
        self.alertLabel.hidden = YES;
        return 1;
    } else {
        self.alertLabel.hidden = NO;
        return 0;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.mediaArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellID = @"longCell";
    
    PLLiveTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellID];
    if (!cell) {
        cell = [[PLLiveTableViewCell alloc] initWithStyle:(UITableViewCellStyleDefault) reuseIdentifier:cellID];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
        cell.backgroundColor = [UIColor whiteColor];
    }
    if (indexPath.row < self.mediaArray.count) {
        cell.media = [self.mediaArray objectAtIndex:indexPath.row];
    }
    return cell;
}

- (CGFloat)tableView:(UITableView *)tableView estimatedHeightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return 44;
}

- (CGFloat)tableView:(UITableView *)tableView heightForRowAtIndexPath:(NSIndexPath *)indexPath {
    return UITableViewAutomaticDimension;
}

- (CGFloat)tableView:(UITableView *)tableView heightForHeaderInSection:(NSInteger)section {
    return .1;
}

- (CGFloat)tableView:(UITableView *)tableView heightForFooterInSection:(NSInteger)section {
    return .1;
}

- (UIView *)tableView:(UITableView *)tableView viewForFooterInSection:(NSInteger)section {
    return nil;
}

- (UIView *)tableView:(UITableView *)tableView viewForHeaderInSection:(NSInteger)section {
    return nil;
}

- (void)tableView:(UITableView *)tableView didSelectRowAtIndexPath:(NSIndexPath *)indexPath {
    PLMediaInfo *media = [self.mediaArray objectAtIndex:indexPath.row];
    PLPlayViewController *playController = [[PLPlayViewController alloc] init];
    playController.url = [NSURL URLWithString:media.videoURL];
    playController.thumbImageURL = [NSURL URLWithString:media.thumbURL];
    [self presentViewController:playController animated:YES completion:nil];
}

- (void)codeViewController:(PLQRCodeViewController *)codeController scanResult:(NSString *)result {
    if (!result) return;
    
    PLPlayViewController *playController = [[PLPlayViewController alloc] init];
    playController.url = [NSURL URLWithString:result];
    [self presentViewController:playController animated:YES completion:nil];
    
}


@end
