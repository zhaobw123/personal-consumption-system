#include <stdio.h>
#include <string.h>
#include <ctype.h>  // 用于月份格式校验
#define MAX 100  
#define DATA_FILE "consumption_records.dat"  // 数据持久化文件
#define MAX_CATEGORY 20  // 类别占比统计-最大支持类别数
#include <stdlib.h>  // 新增，用于 system("cls")
#include <conio.h>   // 新代码，用于 _getch()
#include <windows.h> // 用于调用 Windows API 改变控制台颜色以实现高亮

// 消费记录结构体定义（统一字段名，适配所有模块）
typedef struct {
    char date[15];    // 消费日期 YYYY-MM-DD
    char type[30];    // 消费类别
    float amount;     // 消费金额
    char remark[50];  // 消费备注
} Record;
// ====================== 新增功能：高亮效果 ======================
// ---------------------------------------------------------------------------------------------------------------------------------- 杨：高亮效果（开始） ------------------------------
#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

const char *menuItems[] = {
    "1. 新增消费记录",
    "2. 删除消费记录",
    "3. 查询消费记录",
    "4. 修改消费记录",
    "5. 保存记录到文件",
    "6. 月度消费汇总",
    "7. 月度类别占比",
    "8. 最高/最低消费查询",
    "9. 退出系统"
};
const int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

int selectMenu() {
    int index = 0; // 当前高亮菜单
    int ch;
    int i; 
    // 获取控制台标准输出句柄，用于后面修改颜色
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE); 
    //==================================================================
    while (1) {
        system("cls"); // 清屏
        printf("==================== 学生消费管理系统 ====================\n");
        for (i = 0; i < menuCount; i++) {
            if (i == index) {
                // AI 修改高亮显示样式开始
                // 功能效果：当菜单被选中时，背景变为淡蓝色(9)，字体变为白色(F)。
                SetConsoleTextAttribute(hConsole, 0x9F); // 0x9F 代表 淡蓝底白字 高亮
                printf("  -> %s  \n", menuItems[i]);
                SetConsoleTextAttribute(hConsole, 0xF0); // 恢复原本的白底黑字(0xF0)
            } else {
                printf("     %s\n", menuItems[i]);
            }
        }
        printf("==========================================================\n");
        printf("使用 ↑ ↓ 方向键选择，回车确认\n");

        ch = _getch(); // 获取按键
        if (ch == 0 || ch == 224) { // 方向键返回两个值
            ch = _getch();
            if (ch == KEY_UP) {
                index--;
                if (index < 0) index = menuCount - 1; // 循环到底部
            } else if (ch == KEY_DOWN) {
                index++;
                if (index >= menuCount) index = 0; // 循环到顶部
            }
        } else if (ch == KEY_ENTER) {
            return index + 1; // 返回选项编号
        }
    }
}
// 全局变量（所有模块共用）
Record records[MAX];  
int recordCount = 0;  
// ------------------------------------------------------------------------------------------------------------------------------杨：高亮效果（结束）------------------------------------
// ====================== 记录操作模块（原有核心功能） ======================
//----------------------------------------------------------------------------------------------------------------------------杨：新增消费记录（开始）-------------------------------------
// 新增消费记录函数（支持多条新增）
void addRecord() {
    char choice; // 用于选择是否继续新增
    do {
        if (recordCount >= MAX) {
            printf("记录已满，无法新增。\n");
            return;
        }

        printf("请输入消费时间：");
        scanf("%s", records[recordCount].date);

        printf("请输入消费金额：");
        scanf("%f", &records[recordCount].amount);

        // 清理scanf残留换行符，避免影响后续输入
        getchar();

        printf("请输入消费类别：");
        fgets(records[recordCount].type, sizeof(records[recordCount].type), stdin);
        records[recordCount].type[strcspn(records[recordCount].type, "\n")] = '\0';

        printf("请输入备注：");
        fgets(records[recordCount].remark, sizeof(records[recordCount].remark), stdin);
        records[recordCount].remark[strcspn(records[recordCount].remark, "\n")] = '\0';

        recordCount++;
        printf("新增成功。\n");

        // 询问是否继续新增
        printf("是否继续新增记录？(y/n)：");
        scanf(" %c", &choice); // 空格吸收换行符
        getchar(); // 清理输入缓冲区
    } while (choice == 'y' || choice == 'Y'); // 循环新增直到选择n
}

//--------------------------------------------------------------------------------------------------------------------------杨：新增消费记录（结束）----------------------------------

//--------------------------------------------------------------------------------------------------------------------------余：删除消费记录（开始）----------------------------------              
// 核心功能：删除消费记录（严格按需求实现）
void deleteRecord() {
    char targetTime[20];  // 存储要删除的目标时间
    int found = 0;        // 标记是否找到目标记录
    int i, j;

    // 1. 删除前打印所有消费记录
    printf("\n==================== 当前消费记录 ====================\n");
    if (recordCount == 0) {
        printf("暂无任何消费记录！\n");
        return;  // 没有记录直接退出函数
    }
    for (i = 0; i < recordCount; i++) {
        printf("第%d条\n", i + 1);
        printf("时间：%s\n", records[i].date);
        printf("类别：%s\n", records[i].type);
        printf("金额：%.2f\n", records[i].amount);
        printf("备注：%s\n", records[i].remark);
        printf("----------------------------------------------------\n");
    }
    // 2. 提示用户输入要删除的时间（修复输入异常问题）
    printf("\n请输入要删除的记录时间（格式：YYYY-MM-DD）：");
    // 读取带横线的时间字符串（支持空格，若不需要空格可改用%s）
    fgets(targetTime, sizeof(targetTime), stdin);
    // 去除fgets读取的末尾换换行符
    targetTime[strcspn(targetTime, "\n")] = '\0';

    // 3. 遍历数组查找目标记录
    for (i = 0; i < recordCount; i++) {
        if (strcmp(records[i].date, targetTime) == 0) {
            found = 1;
            // 4. 后面的记录向前移动，覆盖要删除的记录
            for (j = i; j < recordCount - 1; j++) {
                records[j] = records[j + 1];
            }
            // 记录数减1
            recordCount--;
            printf("删除成功！已移除时间为【%s】的记录\n", targetTime);
            break;  // 若要删除所有重复时间，可去掉break
        }
    }

    // 5. 未找到目标记录的提示
    if (!found) {
        printf("未检测到时间为【%s】的消费记录\n", targetTime);
    }
}

//--------------------------------------------------------------------------------------------------------（测试删除效果） 
//
////测试数据初始化函数（后续直接删除/注释即可）
//void initTestData() {
//    // 初始化测试数据（适配统计分析模块）
//    strcpy(records[0].date, "2026-03-15");
//    strcpy(records[0].type, "餐饮");
//    records[0].amount = 200.50;
//    strcpy(records[0].remark, "午餐");
//
//    strcpy(records[1].date, "2026-04-20");
//    strcpy(records[1].type, "购物");
//    records[1].amount = 150.00;
//    strcpy(records[1].remark, "买衣服");
//
//    strcpy(records[2].date, "2026-03-25");
//    strcpy(records[2].type, "娱乐");
//    records[2].amount = 300.00;
//    strcpy(records[2].remark, "看电影");
//
//    strcpy(records[3].date, "2026-03-10");
//    strcpy(records[3].type, "餐饮");
//    records[3].amount = 50.00;
//    strcpy(records[3].remark, "早餐");
//
//    recordCount = 4;
//    printf("测试数据初始化完成！\n");
//}
//------------------------------------------------------------------------------------------------------------------------余：删除消费记录（结束）-------------------------------------- 
// --------------------------------------------------------------------------------------------------------------------赵：查询和修改消费记录（开始）-----------------------------------
int searchRecord() {
    int choice, count = 0,i;
    char keyword[30];
    printf("\n===== 消费记录查询 =====\n");
    printf("1. 按日期查询\n2. 按类别查询\n请选择：");
    scanf("%d", &choice);
    getchar();  // 吸收换行符
    if (choice == 1) {
        printf("请输入查询日期（YYYY-MM-DD）：");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';
        printf("\n===== 查询结果 =====\n");
        for (i = 0; i < recordCount; i++) {
            if (strcmp(records[i].date, keyword) == 0) {
                printf("序号：%d | 日期：%s | 类别：%s | 金额：%.2f | 备注：%s\n",
                       i+1, records[i].date, records[i].type, records[i].amount, records[i].remark);
                count++;
            }
        }
    } else if (choice == 2) {
        printf("请输入查询类别：");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';
        printf("\n===== 查询结果 =====\n");
        for (i = 0; i < recordCount; i++) {
            if (strcmp(records[i].type, keyword) == 0) {
                printf("序号：%d | 日期：%s | 类别：%s | 金额：%.2f | 备注：%s\n",
                       i+1, records[i].date, records[i].type, records[i].amount, records[i].remark);
                count++;
            }
        }
    } else {
        printf("无效选择！\n");
        return 0;
    }
    if (count == 0) {
        printf("未找到匹配的消费记录！\n");
    } else {
        printf("共查询到 %d 条匹配记录\n", count);
    }
    return count;
}
// 修改消费记录
int modifyRecord() {
    char targetDate[15];
    int index = -1,i;
    printf("\n===== 消费记录修改 =====\n");
    printf("请输入要修改的记录日期（YYYY-MM-DD）：");
    scanf("%s", targetDate);
    // 遍历定位目标记录
    for (i = 0; i < recordCount; i++) {
        if (strcmp(records[i].date, targetDate) == 0) {
            index = i;
            break;
        }
    }
    if (index == -1) {
        printf("未找到该日期的消费记录！\n");
        return 0;
    }
    // 展示原记录并选择修改项
    int choice;
    printf("找到目标记录：\n");
    printf("日期：%s | 类别：%s | 金额：%.2f | 备注：%s\n",
           records[index].date, records[index].type, records[index].amount, records[index].remark);
    printf("请选择修改项：1.类别 2.金额 3.备注 4.全部修改\n请选择：");
    scanf("%d", &choice);
    getchar();
    switch (choice) {
        case 1:
            printf("输入新类别：");
            fgets(records[index].type, sizeof(records[index].type), stdin);
            records[index].type[strcspn(records[index].type, "\n")] = '\0';
            break;
        case 2:
            printf("输入新金额：");
            scanf("%f", &records[index].amount);
            break;
        case 3:
            printf("输入新备注：");
            fgets(records[index].remark, sizeof(records[index].remark), stdin);
            records[index].remark[strcspn(records[index].remark, "\n")] = '\0';
            break;
        case 4:
            printf("新类别："); 
            fgets(records[index].type, sizeof(records[index].type), stdin);
            records[index].type[strcspn(records[index].type, "\n")] = '\0';
            printf("新金额："); 
            scanf("%f", &records[index].amount);
            getchar();
            printf("新备注："); 
            fgets(records[index].remark, sizeof(records[index].remark), stdin);
            records[index].remark[strcspn(records[index].remark, "\n")] = '\0';
            break;
        default:
            printf("无效选择，修改失败！\n");
            return 0;
    }
    printf("记录修改成功！\n");
    return 1;
}
//-----------------------------------------------------------------------------------------------------------------------------赵：查询和修改消费记录（结束）------------------------- 
// ---------------------------------------------------------------------------------------------------------------------------------- 杨：月度消费汇总（开始） ------------------------------
// ====================== 统计分析模块 ======================
// 通用工具函数：月份格式校验（支撑统计分析模块）
int isValidMonthFormat(const char *month) {
    if (strlen(month) != 7) return 0;
    if (month[4] != '-') return 0;
    int i; 
    for ( i = 0; i < 4; i++) {
        if (!isdigit(month[i])) return 0;
    }
    int mon = atoi(&month[5]);
    if (mon < 1 || mon > 12) return 0;
    return 1;
}

void monthlySummary() {
    char month[8];
    int inputValid = 0;

    // 循环获取有效月份（格式错误则重新输入）
    while (!inputValid) {
        printf("请输入要汇总的月份（格式：YYYY-MM）：");
        scanf("%7s", month);
        getchar();  // 清理输入缓冲区的换行符

        if (isValidMonthFormat(month)) {
            inputValid = 1;
        } else {
            printf("? 月份格式错误！请按 YYYY-MM 输入（例如2026-03），月份范围1-12\n");
        }
    }

    float sum = 0.0;    // 月度总消费
    int recordNum = 0,i;  // 月度记录数

    // 遍历记录统计数据
    for (i = 0; i < recordCount; i++) {
        char dateMonth[8];
        strncpy(dateMonth, records[i].date, 7);  // 截取date前7位（YYYY-MM）
        dateMonth[7] = '\0';

        if (strcmp(dateMonth, month) == 0) {
            sum += records[i].amount;
            recordNum++;
        }
    }

    // 输出结果
    printf("\n==================== %s 月度消费汇总 ====================\n", month);
    if (recordNum == 0) {
        printf("?? 该月份无任何消费记录！\n");
    } else {
        float avg = sum / recordNum;  // 平均单笔消费
        printf("?? 统计结果：\n");
        printf("   月度消费记录数：%d 条\n", recordNum);
        printf("   月度总消费金额：%.2f 元\n", sum);
        printf("   月度平均单笔消费：%.2f 元\n", avg);
    }
    printf("========================================================\n");
}
// ---------------------------------------------------------------------------------------------------------------------------杨：月度消费汇总（结束） --------------------------------

// -------------------------------------------------------------------------------------------------------------------------- 余：月度类别占比（开始） -------------------------------
// 类别统计相关全局变量
char categories[MAX_CATEGORY][30];  // 存储不重复的类别
float catSum[MAX_CATEGORY];         // 各类别总金额
int catCount = 0;                   // 类别数量

void calculateMonthlyCategoryRatio() {
    char targetMonth[8];  // 存储用户输入的目标月份（YYYY-MM）
    int inputValid = 0;

    // 获取并校验用户输入的月份
    while (!inputValid) {
        printf("请输入要统计的月份（格式：YYYY-MM）：");
        scanf("%7s", targetMonth);
        if (isValidMonthFormat(targetMonth)) {
            inputValid = 1;
        } else {
            printf("格式错误！请按 YYYY-MM 输入（例如2026-03），月份1-12\n");
        }
    }

    float total = 0.0;  // 目标月份的总消费金额
    int i, j;
    int isExist;

    // 重置统计数据
    catCount = 0;
    memset(categories, 0, sizeof(categories));
    memset(catSum, 0, sizeof(catSum));

    // 遍历记录：仅统计「目标月份」的记录
    for (i = 0; i < recordCount; i++) {
        char recordMonth[8];
        strncpy(recordMonth, records[i].date, 7);  // 截取date的前7位（YYYY-MM）
        recordMonth[7] = '\0';

        // 只处理目标月份的记录
        if (strcmp(recordMonth, targetMonth) != 0) {
            continue;
        }

        total += records[i].amount;  // 累加目标月份总金额

        // 类别去重+金额累加
        isExist = 0;
        for (j = 0; j < catCount; j++) {
            if (strcmp(records[i].type, categories[j]) == 0) {
                catSum[j] += records[i].amount;
                isExist = 1;
                break;
            }
        }

        if (!isExist && catCount < MAX_CATEGORY) {
            strcpy(categories[catCount], records[i].type);
            catSum[catCount] = records[i].amount;
            catCount++;
        } else if (!isExist && catCount >= MAX_CATEGORY) {
            printf("警告：类别数量已达上限，无法统计【%s】\n", records[i].type);
        }
    }

    // 输出结果
    printf("\n==================== %s 月度类别占比统计 ====================\n", targetMonth);
    if (total == 0.0 || catCount == 0) {
        printf("该月无消费记录，无法统计类别占比！\n");
        printf("========================================================\n");
        return;
    }

    printf("%-10s %-12s %-8s\n", "类别", "类别总金额(元)", "占比(%)");
    printf("--------------------------------------------------------\n");
    for (i = 0; i < catCount; i++) {
        float ratio = (catSum[i] / total) * 100;
        printf("%-10s %-12.2f %-8.1f\n", categories[i], catSum[i], ratio);
    }
    printf("--------------------------------------------------------\n");
    printf("%-10s %-12.2f %-8.1f\n", "总计", total, 100.0);
    printf("========================================================\n");
}
// ----------------------------------------------------------------------------------------------------------------------- 余：月度类别占比（结束） -----------------------------------

// ------------------------------------------------------------------------------------------------------------------- 赵：最高/最低消费查询 （开始）----------------------------------
void queryMaxMinConsumption() {
    // 校验是否有消费记录
    if (recordCount == 0) {
        printf("\n===== 最高/最低消费查询 =====\n");
        printf("暂无任何消费记录，无法查询！\n");
        return;
    }
    // 初始化最高/最低消费的索引和金额（默认第一条记录）
    int maxIndex = 0, minIndex = 0,i;
    float maxAmount = records[0].amount;
    float minAmount = records[0].amount;
    // 遍历所有记录，找出最高和最低消费
    for ( i = 1; i < recordCount; i++) {
        // 更新最高消费
        if (records[i].amount > maxAmount) {
            maxAmount = records[i].amount;
            maxIndex = i;
        }
        // 更新最低消费（排除金额为0的情况）
        if (records[i].amount < minAmount && records[i].amount > 0) {
            minAmount = records[i].amount;
            minIndex = i;
        }
    }
    printf("\n===== 最高/最低消费查询结果 =====\n");
    // 最高消费详情
    printf("【最高消费记录】\n");
    printf("日期：%s\n", records[maxIndex].date);
    printf("类别：%s\n", records[maxIndex].type);
    printf("金额：%.2f 元\n", records[maxIndex].amount);
    printf("备注：%s\n", records[maxIndex].remark);
    printf("----------------------------------\n");
    // 最低消费详情
    printf("【最低消费记录】\n");
    printf("日期：%s\n", records[minIndex].date);
    printf("类别：%s\n", records[minIndex].type);
    printf("金额：%.2f 元\n", records[minIndex].amount);
    printf("备注：%s\n", records[minIndex].remark);
    printf("==================================\n");
}
// --------------------------------------------------------------------------------------------------------------------------- 赵：最高/最低消费查询（结束）--------------------------
// ====================== 交互与持久化模块 ======================
// ------------------------------------------------------------------------------------------------------------------------------赵：文件保存与加载（开始） ------------------------------
// 从文件加载记录（程序启动时自动加载）
void loadRecordsFromFile() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("?? 首次运行，未检测到数据文件（%s），将创建新文件\n", DATA_FILE);
        return;
    }

    // 先读取记录数
    fread(&recordCount, sizeof(int), 1, fp);
    // 再读取所有记录
    fread(records, sizeof(Record), recordCount, fp);
    fclose(fp);

    printf("? 成功从文件加载 %d 条消费记录\n", recordCount);
}
// 将记录保存到文件（程序退出/手动保存时调用）
void saveRecordsToFile() {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("? 保存失败！无法打开文件（%s）\n", DATA_FILE);
        return;
    }
    // 先写入记录数
    fwrite(&recordCount, sizeof(int), 1, fp);
    // 再写入所有记录
    fwrite(records, sizeof(Record), recordCount, fp);
    fclose(fp);

    printf("? 成功保存 %d 条消费记录到文件（%s）\n", recordCount, DATA_FILE);
}
// ------------------------------------------------------------------------------------------------------------------------------- 赵：文件保存与加载（结束） ------------------------------
// --------------------------------------------------------------------------------------------------------------------------- 余：方向键选择和回车确认（开始） ------------------------------
// 主菜单交互（整合所有功能，支持方向键选择）

void showMainMenu() {
    int choice;
    do {
//==============================================================使用方向键选择菜单
        choice = selectMenu(); 
        switch (choice) {
            case 1: addRecord(); break;
            case 2: deleteRecord(); break; 
            case 3: searchRecord(); break; 
            case 4: modifyRecord(); break;
            case 5: saveRecordsToFile(); break; 
            case 6: monthlySummary(); break; 
            case 7: calculateMonthlyCategoryRatio(); break;
            case 8: queryMaxMinConsumption(); break; 
            case 9:
                saveRecordsToFile();
                printf("系统已退出，所有记录已自动保存！\n");
                break;
            default:
                printf("输入无效，请选择1-9之间的数字！\n");
        }
//================================================================使用方向键选择菜单结束
        
        // 功能效果：执行完某项功能后（例如查询、统计完成），不立刻清屏刷新菜单。
        // 而是给出一个类似“按任意键返回主菜单”的提示，防止查询结果被清屏冲掉。 
        if (choice != 9) {
            printf("\n操作执行完毕，按任意键返回主菜单...");
            _getch(); // 等待用户按任意键再进入下一次循环
        }
 
    } while (choice != 9);
}
// ------------------------------------------------------------------------------------------------------------------------------ 余：方向键选择和回车确认（结束） ------------------------------
// 主函数
int main() {
    system("color F0");
    int i; 
    // 程序启动时自动加载文件中的记录
    loadRecordsFromFile();
    // initTestData();//调试用的数据，正式使用时删除
    // 启动主菜单
    showMainMenu();  
    return 0;
}
