//
//  PLLongVideoViewController.m
//  NiuPlayer
//
//  Created by hxiongan on 2018/3/1.
//  Copyright © 2018年 hxiongan. All rights reserved.
//

#import "PLLongVideoViewController.h"
#import "PLLongMediaTableViewCell.h"

@interface PLLongVideoViewController ()
<
UITableViewDelegate,
UITableViewDataSource,
PLLongMediaTableViewCellDelegate,
PLCodeViewControllerDelegate
>

@property (nonatomic, strong) UITableView *tableView;

@property (nonatomic, strong) NSArray *mediaArray;

@property (nonatomic, weak) PLLongMediaTableViewCell *playingCell;

@property (nonatomic, assign) BOOL isFullScreen;

@end

@implementation PLLongVideoViewController

- (void)onUIApplication:(BOOL)active {
    if (self.playingCell) {
        [self.playingCell configureVideo:active];
    }
}

- (void)viewDidLoad {
    [super viewDidLoad];
    
    self.title = @"长视频";
    
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
}

- (void)didReceiveMemoryWarning {
    [super didReceiveMemoryWarning];
    // Dispose of any resources that can be recreated.
}

- (void)viewDidDisappear:(BOOL)animated {
    [super viewDidDisappear:animated];
    [self stop];
}

- (BOOL)prefersStatusBarHidden {
    return self.isFullScreen;
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
    
    [self.view showLoadingHUD];
    __weak typeof(self) wself = self;
    [PLHttpSession requestLongMediaList:^(NSArray *list, NSError *error) {
        [wself.view hideLoadingHUD];

        wself.mediaArray = list;

        [wself.tableView reloadData];
        dispatch_after(dispatch_time(DISPATCH_TIME_NOW, (int64_t)(.2 * NSEC_PER_SEC)), dispatch_get_main_queue(), ^{
            [wself playTopCell];
        });
        
        if (0 == wself.mediaArray.count && error) {
            [wself.view showTip:[NSString stringWithFormat:@"%@", error.description]];
        }
    }];
}

- (void)stop {
    
    NSArray *array = [self.tableView visibleCells];

    for (PLLongMediaTableViewCell *cell in array) {
        [cell stop];
    }
}

// 根据cell的位置，决定播放哪个cell
- (void)playTopCell {
    
    if (self.playingCell) return;
    
    NSArray *array = [self.tableView visibleCells];
    
    PLLongMediaTableViewCell *playCell = nil;
    CGFloat minOriginY = self.view.bounds.size.height;
    CGFloat navigationBarHeight = 20 + self.navigationController.navigationBar.bounds.size.height;
    for (PLLongMediaTableViewCell *cell in array) {
        CGRect rc = [self.tableView convertRect:cell.frame toView:self.view];
        rc.size.height -= [PLLongMediaTableViewCell headerViewHeight];
        if (rc.origin.y > navigationBarHeight && rc.origin.y + rc.size.height < self.view.bounds.size.height) {
            if (rc.origin.y < minOriginY) {
                minOriginY = rc.origin.y;
                playCell = cell;
            }
            break;
        }
    }
    
    self.playingCell = playCell;
    [playCell play];
}

- (void)tableViewWillPlay:(PLLongMediaTableViewCell *)cell {
    if (cell == self.playingCell) return;
    
    NSArray *array = [self.tableView visibleCells];
    for (PLLongMediaTableViewCell *tempCell in array) {
        if (cell != tempCell) {
            [tempCell stop];
        }
    }
    self.playingCell = cell;
}

- (void)tableViewCellEnterFullScreen:(PLLongMediaTableViewCell *)cell {
    self.isFullScreen = YES;
    [self setNeedsStatusBarAppearanceUpdate];
}

- (void)tableViewCellExitFullScreen:(PLLongMediaTableViewCell *)cell {
    self.isFullScreen = NO;
    [self setNeedsStatusBarAppearanceUpdate];
}

- (void)scrollViewDidEndDragging:(UIScrollView *)scrollView willDecelerate:(BOOL)decelerate {

    if (decelerate) return;
    if (nil == self.playingCell) {
        [self playTopCell];
    }
}

- (void)scrollViewDidEndDecelerating:(UIScrollView *)scrollView {
    if (nil == self.playingCell) {
        [self playTopCell];
    }
}

- (NSInteger)numberOfSectionsInTableView:(UITableView *)tableView {
    if (self.mediaArray.count) {
        [self hideReloadButton];
        return 1;
    } else {
        [self showReloadButton];
        return 0;
    }
}

- (NSInteger)tableView:(UITableView *)tableView numberOfRowsInSection:(NSInteger)section {
    return self.mediaArray.count;
}

- (UITableViewCell *)tableView:(UITableView *)tableView cellForRowAtIndexPath:(NSIndexPath *)indexPath {
    static NSString *cellID = @"longCell";
    
    PLLongMediaTableViewCell *cell = [tableView dequeueReusableCellWithIdentifier:cellID];
    if (!cell) {
        cell = [[PLLongMediaTableViewCell alloc] initWithStyle:(UITableViewCellStyleDefault) reuseIdentifier:cellID];
        cell.selectionStyle = UITableViewCellSelectionStyleNone;
        cell.delegate = self;
        cell.backgroundColor = [UIColor whiteColor];
    }
    if (indexPath.row < self.mediaArray.count) {
        cell.media = [self.mediaArray objectAtIndex:indexPath.row];
    }
    return cell;
}

- (void)tableView:(UITableView *)tableView didEndDisplayingCell:(UITableViewCell *)cell forRowAtIndexPath:(NSIndexPath *)indexPath {
    if (cell == self.playingCell) {
        [self.playingCell stop];
        self.playingCell = nil;
    }
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

- (void)codeViewController:(PLQRCodeViewController *)codeController scanResult:(NSString *)result {
    if (!result) return;
    
    PLPlayViewController *playController = [[PLPlayViewController alloc] init];
    playController.url = [NSURL URLWithString:result];
    [self presentViewController:playController animated:YES completion:nil];

}

@end
