#include <stdio.h>
#include <string.h>
#include <ctype.h>
#include <stdlib.h>  // 用于system函数/exit函数
#define MAX 100  
#define DATA_FILE "consumption_records.dat"
#define MAX_CATEGORY 20
#define MAX_RECORDS 100  

// 消费记录结构体
typedef struct {
    char date[15];    // YYYY-MM-DD
    char type[30];    // 消费类别
    float amount;     // 消费金额
    char remark[50];  // 备注
} Record;

// 全局变量
Record records[MAX];  
int recordCount = 0,i,j;  
// ====================== 杨：预算管理模块 start ======================
float budget = 0;  // ：月度消费预算
// 设置月度消费预算
void setBudget(float newBudget) {
    budget = newBudget;
    printf("月度消费预算已设置为：%.2f 元\n", newBudget);
}

// 计算总消费
float calculateTotal() {
    float total = 0;
    for (i = 0; i < recordCount; i++) {
        total += records[i].amount;
    }
    return total;
}

// 检查是否超出预算
void checkBudget() {
    if (budget <= 0) {
        printf("请先设置月度消费预算！\n");
        return;
    }
    float total = calculateTotal();
    if (total > budget) {
        printf("?? 本月消费超出预算 %.2f 元！\n", total - budget);
    } else {
        printf("? 本月消费未超出预算，剩余可用金额：%.2f 元\n", budget - total);
    }
}
// ====================== 杨：预算管理模块 end ========================
// ====================== 余：批量操作模块 start ======================
//批量删除某类消费记录
void batchDeleteByCategory() {
    char targetCategory[30];
    int deleteCount = 0;
    int i, j;

    if (recordCount == 0) {
        printf("\n===== 批量删除某类消费 =====\n");
        printf("暂无任何消费记录，无法批量删除！\n");
        return;
    }

    // 输入目标类别
    printf("\n===== 批量删除某类消费 =====\n");
    printf("请输入要删除的消费类别（例：娱乐）：");
    fgets(targetCategory, sizeof(targetCategory), stdin);
    targetCategory[strcspn(targetCategory, "\n")] = '\0'; 

    // 遍历记录，筛选保留非目标类别的记录（间接实现批量删除）
    for (i = 0, j = 0; i < recordCount; i++) {
        if (strcmp(records[i].type, targetCategory) != 0) {
            records[j] = records[i]; // 保留不匹配的记录
            j++;
        } else {
            deleteCount++; // 统计删除数量
        }
    }

    // 更新记录总数
    recordCount = j;

    // 输出结果
    if (deleteCount > 0) {
        printf("批量删除成功！共删除 %d 条【%s】类消费记录\n", deleteCount, targetCategory);
    } else {
        printf("未找到【%s】类的消费记录，删除失败！\n", targetCategory);
    }
}

//余：批量修改某类消费的备注
void batchModifyRemarkByCategory() {
    char targetCategory[30];
    char newRemark[50];
    int modifyCount = 0;
    int i;

    if (recordCount == 0) {
        printf("\n===== 批量修改某类备注 =====\n");
        printf("暂无任何消费记录，无法批量修改！\n");
        return;
    }

    // 输入目标类别和新备注
    printf("\n===== 批量修改某类备注 =====\n");
    printf("请输入要修改的消费类别（例：餐饮）：");
    fgets(targetCategory, sizeof(targetCategory), stdin);
    targetCategory[strcspn(targetCategory, "\n")] = '\0';

    printf("请输入统一的新备注（例：日常开销）：");
    fgets(newRemark, sizeof(newRemark), stdin);
    newRemark[strcspn(newRemark, "\n")] = '\0';

    // 遍历记录，修改目标类别的备注
    for (i = 0; i < recordCount; i++) {
        if (strcmp(records[i].type, targetCategory) == 0) {
            strcpy(records[i].remark, newRemark);
            modifyCount++;
        }
    }

    // 输出结果
    if (modifyCount > 0) {
        printf("批量修改成功！共修改 %d 条【%s】类消费记录的备注\n", modifyCount, targetCategory);
    } else {
        printf("未找到【%s】类的消费记录，修改失败！\n", targetCategory);
    }
}

//批量操作子菜单
void batchOperationMenu() {
    int choice;
    printf("\n===== 批量操作子菜单 =====\n");
    printf("1. 批量删除某类消费记录\n");
    printf("2. 批量修改某类消费备注\n");
    printf("3. 返回主菜单\n"); 
    printf("请选择功能（1-3）：");
    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 3) {
        printf("输入无效！请选择1-3之间的数字：");
        while (getchar() != '\n');
    }
    getchar(); // 清理换行符

    switch (choice) {
        case 1:
            batchDeleteByCategory();
            break;
        case 2:
            batchModifyRemarkByCategory();
            break;
        case 3:
            printf("返回主菜单...\n");
            break;
        default:
            printf("输入无效，请选择1-3之间的数字！\n");
    }
}
// ====================== 余：批量操作模块  end======================

//校验日期格式
int isValidDateFormat(const char *date) {
    if (strlen(date) != 10) return 0;
    if (date[4] != '-' || date[7] != '-') return 0;
    // 校验年、月、日是否为数字
    for (i = 0; i < 10; i++) {
        if (i == 4 || i == 7) continue;
        if (!isdigit(date[i])) return 0;
    }
    // 简单校验月份和日期范围
    int year = atoi(date);
    int month = atoi(date + 5);
    int day = atoi(date + 8);
    if (month < 1 || month > 12) return 0;
    if (day < 1 || day > 31) return 0; 
    return 1;
}

//校验月份格式（YYYY-MM）
int isValidMonthFormat(const char *month) {
    if (strlen(month) != 7) return 0;
    if (month[4] != '-') return 0;
    for ( i = 0; i < 4; i++) {
        if (!isdigit(month[i])) return 0;
    }
    int mon = atoi(&month[5]);
    if (mon < 1 || mon > 12) return 0;
    return 1;
}

// 工具函数：显示所有记录
void showAllRecords() {
    printf("\n==================== 当前所有消费记录 ====================\n");
    if (recordCount == 0) {
        printf("暂无任何消费记录！\n");
        printf("==========================================================\n");
        return;
    }
    for (i = 0; i < recordCount; i++) {
        printf("第%d条\n", i + 1);
        printf("时间：%s\n", records[i].date);
        printf("类别：%s\n", records[i].type);
        printf("金额：%.2f\n", records[i].amount);
        printf("备注：%s\n", records[i].remark);
        printf("--------------------------------------------------------\n");
    }
    printf("==========================================================\n");
}

// 新增消费记录
void addRecord() {
    char choice;
    do {
        if (recordCount >= MAX) {
            printf("记录已满（最大%d条），无法新增。\n", MAX);
            return;
        }

        // 日期输入+校验
        char tempDate[15];
        do {
            printf("请输入消费日期（YYYY-MM-DD）：");
            scanf("%14s", tempDate);
            getchar(); 
            if (!isValidDateFormat(tempDate)) {
                printf("日期格式错误！请按YYYY-MM-DD输入（例如2026-03-15）\n");
            } else {
                strcpy(records[recordCount].date, tempDate);
                break;
            }
        } while (1);

        // 金额输入+校验
        float tempAmount;
        do {
            printf("请输入消费金额：");
            if (scanf("%f", &tempAmount) != 1 || tempAmount <= 0) {
                printf("金额输入错误！请输入大于0的数字\n");
                // 清空输入缓冲区错误内容
                while (getchar() != '\n');
            } else {
                records[recordCount].amount = tempAmount;
                getchar(); 
                break;
            }
        } while (1);

        // 类别输入
        printf("请输入消费类别：");
        fgets(records[recordCount].type, sizeof(records[recordCount].type), stdin);
        records[recordCount].type[strcspn(records[recordCount].type, "\n")] = '\0';

        // 备注输入
        printf("请输入备注：");
        fgets(records[recordCount].remark, sizeof(records[recordCount].remark), stdin);
        records[recordCount].remark[strcspn(records[recordCount].remark, "\n")] = '\0';

        recordCount++;
        printf("新增成功！\n");

        printf("是否继续新增记录？(y/n)：");
        scanf(" %c", &choice);
        getchar();
    } while (choice == 'y' || choice == 'Y');
}

// 删除消费记录
void deleteRecord() {
    showAllRecords(); // 删除前先显示所有记录
    if (recordCount == 0) return;

    char targetTime[20];
    printf("\n请输入要删除的记录时间（YYYY-MM-DD）：");
    fgets(targetTime, sizeof(targetTime), stdin);
    targetTime[strcspn(targetTime, "\n")] = '\0';

    if (!isValidDateFormat(targetTime)) {
        printf("日期格式错误！请按YYYY-MM-DD输入\n");
        return;
    }

    int deleteCount = 0;
    // 反向遍历删除
    for (i = recordCount - 1; i >= 0; i--) {
        if (strcmp(records[i].date, targetTime) == 0) {
            // 后面的记录向前移动
            for ( j = i; j < recordCount - 1; j++) {
                records[j] = records[j + 1];
            }
            recordCount--;
            deleteCount++;
        }
    }

    if (deleteCount > 0) {
        printf("删除成功！共移除 %d 条时间为【%s】的记录\n", deleteCount, targetTime);
    } else {
        printf("未检测到时间为【%s】的消费记录\n", targetTime);
    }
}

// 查询消费记录
int searchRecord() {
    int choice, count = 0;
    char keyword[30];

    printf("\n===== 消费记录查询 =====\n");
    printf("1. 按日期查询\n2. 按类别查询\n请选择：");
    while (scanf("%d", &choice) != 1 || choice < 1 || choice > 2) {
        printf("输入无效！请选择1或2：");
        while (getchar() != '\n'); // 清空错误输入
    }
    getchar();

    if (choice == 1) {
        printf("请输入查询日期（YYYY-MM-DD）：");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';
        if (!isValidDateFormat(keyword)) {
            printf("日期格式错误！\n");
            return 0;
        }
        printf("\n===== 查询结果 =====\n");
        for ( i = 0; i < recordCount; i++) {
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
        for ( i = 0; i < recordCount; i++) {
            if (strcmp(records[i].type, keyword) == 0) {
                printf("序号：%d | 日期：%s | 类别：%s | 金额：%.2f | 备注：%s\n",
                       i+1, records[i].date, records[i].type, records[i].amount, records[i].remark);
                count++;
            }
        }
    }

    if (count == 0) {
        printf("未找到匹配的消费记录！\n");
    } else {
        printf("共查询到 %d 条匹配记录\n", count);
    }
    return count;
}

// 修改消费记录（优化输入校验）
int modifyRecord() {
    char targetDate[15];
    int index = -1;

    printf("\n===== 消费记录修改 =====\n");
    // 日期输入+校验
    do {
        printf("请输入要修改的记录日期（YYYY-MM-DD）：");
        scanf("%14s", targetDate);
        getchar();
        if (!isValidDateFormat(targetDate)) {
            printf("日期格式错误！请按YYYY-MM-DD输入\n");
        } else {
            break;
        }
    } while (1);

    // 定位记录
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

    // 展示原记录
    printf("找到目标记录：\n");
    printf("日期：%s | 类别：%s | 金额：%.2f | 备注：%s\n",
           records[index].date, records[index].type, records[index].amount, records[index].remark);
    
    // 选择修改项
    int choice;
    do {
        printf("请选择修改项：1.类别 2.金额 3.备注 4.全部修改\n请选择：");
        if (scanf("%d", &choice) == 1 && choice >= 1 && choice <= 4) {
            getchar();
            break;
        } else {
            printf("输入无效！请选择1-4：\n");
            while (getchar() != '\n');
        }
    } while (1);

    switch (choice) {
        case 1:
            printf("输入新类别：");
            fgets(records[index].type, sizeof(records[index].type), stdin);
            records[index].type[strcspn(records[index].type, "\n")] = '\0';
            break;
        case 2: {
            float tempAmount;
            do {
                printf("输入新金额：");
                if (scanf("%f", &tempAmount) == 1 && tempAmount > 0) {
                    records[index].amount = tempAmount;
                    getchar();
                    break;
                } else {
                    printf("金额错误！请输入大于0的数字\n");
                    while (getchar() != '\n');
                }
            } while (1);
            break;
        }
        case 3:
            printf("输入新备注：");
            fgets(records[index].remark, sizeof(records[index].remark), stdin);
            records[index].remark[strcspn(records[index].remark, "\n")] = '\0';
            break;
        case 4:
            printf("新类别："); 
            fgets(records[index].type, sizeof(records[index].type), stdin);
            records[index].type[strcspn(records[index].type, "\n")] = '\0';
            
            // 金额校验
            float tempAmount;
            do {
                printf("新金额：");
                if (scanf("%f", &tempAmount) == 1 && tempAmount > 0) {
                    records[index].amount = tempAmount;
                    getchar();
                    break;
                } else {
                    printf("金额错误！请输入大于0的数字\n");
                    while (getchar() != '\n');
                }
            } while (1);
            
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

// -------------------------------杨：月度消费汇总--------------------------------------------------------
void monthlySummary() {
    char month[8];
    int inputValid = 0;

    while (!inputValid) {
        printf("请输入要汇总的月份（格式：YYYY-MM）：");
        scanf("%7s", month);
        getchar();
        if (isValidMonthFormat(month)) {
            inputValid = 1;
        } else {
            printf("月份格式错误！请按 YYYY-MM 输入（例如2026-03）\n");
        }
    }

    float sum = 0.0;
    int recordNum = 0;

    for (i = 0; i < recordCount; i++) {
        char dateMonth[8];
        strncpy(dateMonth, records[i].date, 7);
        dateMonth[7] = '\0';

        if (strcmp(dateMonth, month) == 0) {
            sum += records[i].amount;
            recordNum++;
        }
    }

    printf("\n==================== %s 月度消费汇总 ====================\n", month);
    if (recordNum == 0) {
        printf("该月份无任何消费记录！\n");
    } else {
        float avg = sum / recordNum;
        printf("月度消费记录数：%d 条\n", recordNum);
        printf("月度总消费金额：%.2f 元\n", sum);
        printf("月度平均单笔消费：%.2f 元\n", avg);
    }
    printf("========================================================\n");
}

// ------------------------------------余；月度类别占比统计----------------------------------
char categories[MAX_CATEGORY][30];
float catSum[MAX_CATEGORY];
int catCount = 0;

void calculateMonthlyCategoryRatio() {
    char targetMonth[8];
    int inputValid = 0;

    while (!inputValid) {
        printf("请输入要统计的月份（格式：YYYY-MM）：");
        scanf("%7s", targetMonth);
        getchar();
        if (isValidMonthFormat(targetMonth)) {
            inputValid = 1;
        } else {
            printf("格式错误！请按 YYYY-MM 输入（例如2026-03）\n");
        }
    }

    float total = 0.0;
    catCount = 0;
    memset(categories, 0, sizeof(categories));
    memset(catSum, 0, sizeof(catSum));

    for (i = 0; i < recordCount; i++) {
        char recordMonth[8];
        strncpy(recordMonth, records[i].date, 7);
        recordMonth[7] = '\0';

        if (strcmp(recordMonth, targetMonth) != 0) continue;

        total += records[i].amount;

        int isExist = 0;
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
            printf("警告：类别数量已达上限（%d），无法统计【%s】\n", MAX_CATEGORY, records[i].type);
        }
    }

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

//-----------------------------赵：最高/最低消费查询-------------------------------------------
void queryMaxMinConsumption() {
    if (recordCount == 0) {
        printf("\n===== 最高/最低消费查询 =====\n");
        printf("暂无任何消费记录，无法查询！\n");
        return;
    }

    int maxIndex = 0, minIndex = 0;
    float maxAmount = records[0].amount;
    float minAmount = records[0].amount;

    for ( i = 1; i < recordCount; i++) {
        if (records[i].amount > maxAmount) {
            maxAmount = records[i].amount;
            maxIndex = i;
        }
        if (records[i].amount < minAmount && records[i].amount > 0) {
            minAmount = records[i].amount;
            minIndex = i;
        }
    }

    printf("\n===== 最高/最低消费查询结果 =====\n");
    printf("【最高消费记录】\n");
    printf("日期：%s\n", records[maxIndex].date);
    printf("类别：%s\n", records[maxIndex].type);
    printf("金额：%.2f 元\n", records[maxIndex].amount);
    printf("备注：%s\n", records[maxIndex].remark);
    printf("----------------------------------\n");
    printf("【最低消费记录】\n");
    printf("日期：%s\n", records[minIndex].date);
    printf("类别：%s\n", records[minIndex].type);
    printf("金额：%.2f 元\n", minAmount);
    printf("备注：%s\n", records[minIndex].remark);
    printf("==================================\n");
}

// 从文件加载记录
void loadRecordsFromFile() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (fp == NULL) {
        printf("首次运行，未检测到数据文件，将创建新文件\n");
        return;
    }

    fread(&recordCount, sizeof(int), 1, fp);
    // 防止文件损坏导致记录数超限
    if (recordCount > MAX) recordCount = MAX;
    fread(records, sizeof(Record), recordCount, fp);
    fclose(fp);

    printf("成功从文件加载 %d 条消费记录\n", recordCount);
}

// 保存记录到文件
void saveRecordsToFile() {
    FILE *fp = fopen(DATA_FILE, "wb");
    if (fp == NULL) {
        printf("保存失败！无法打开文件\n");
        return;
    }

    fwrite(&recordCount, sizeof(int), 1, fp);
    fwrite(records, sizeof(Record), recordCount, fp);
    fclose(fp);

    printf("成功保存 %d 条消费记录到文件\n", recordCount);
}
//------------------------------------------------------------------------------------------------------
// 主菜单
void showMainMenu() {
    int choice;
    do {
        printf("\n==================== 学生消费管理系统 ====================\n");
        printf("【记录操作】\n");
        printf("1. 新增消费记录\n");
        printf("2. 删除消费记录\n");
        printf("3. 查询消费记录\n");
        printf("4. 修改消费记录\n");
        printf("5. 显示所有记录\n");
        printf("6. 保存记录到文件\n");
        printf("【预算管理】\n");
        printf("7. 设置月度消费预算\n");
        printf("8. 检查预算使用情况\n");
        printf("【批量操作】\n");
        printf("9. 批量操作（删除/修改类别）\n");
        printf("【统计分析】\n");
        printf("10. 月度消费汇总\n");
        printf("11. 月度类别占比\n");
        printf("12. 最高/最低消费查询\n");
        printf("【系统操作】\n");
        printf("0. 退出系统\n");
        printf("==========================================================\n");
        printf("请选择功能（0-12）：");
        
        // 输入校验
        while (scanf("%d", &choice) != 1 || choice < 0 || choice > 13) {
            printf("输入无效！请选择0-13之间的数字：");
            while (getchar() != '\n');
        }
        getchar();

        switch (choice) {
            case 1: addRecord(); break;
            case 2: deleteRecord(); break;
            case 3: searchRecord(); break;
            case 4: modifyRecord(); break;
            case 5: showAllRecords(); break;
            case 6: saveRecordsToFile(); break;
            // 预算管理功能
            case 7: {
                float newBudget;
                printf("请输入月度消费预算：");
                while (scanf("%f", &newBudget) != 1 || newBudget <= 0) {
                    printf("预算输入错误！请输入大于0的数字：");
                    while (getchar() != '\n');
                }
                getchar();
                setBudget(newBudget);
                break;
            }
            case 8: checkBudget(); break; //杨+ 
            // 批量操作功能
            case 9: batchOperationMenu(); break;//余 
            // 原有统计分析功能
            case 10: monthlySummary(); break;//杨 
            case 11: calculateMonthlyCategoryRatio(); break;//余 
            case 12: queryMaxMinConsumption(); break;//赵 
            // 退出
            case 0:
                saveRecordsToFile();//赵 
                printf("系统已退出，所有记录已自动保存！\n");
                break;
            default:
                printf("输入无效，请重新选择！\n");
        }
    } while (choice != 0);
}

int main() {
    system("color F0"); 
    loadRecordsFromFile();
    showMainMenu(); 
    return 0;
}
