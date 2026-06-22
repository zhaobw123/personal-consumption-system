 #include <stdio.h>
#include <string.h>
#include <ctype.h>  
#define MAX 100
#define DATA_FILE "consumption_records.txt"  
#define MAX_CATEGORY 20  
#include <stdlib.h>  
#include <conio.h>   
#include <windows.h> 

#define KEY_UP 72
#define KEY_DOWN 80
#define KEY_ENTER 13

typedef struct {
    char date[15];    
    char type[30];    
    float amount;     
    char remark[50];  
} Record;
 
int selectMenu(const char **menuItems, int menuCount) {
    int index = 0; 
    int ch;
    int i;
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    while (1) {
        system("cls");
        printf("==================== 学生消费管理系统 ====================\n");
        for (i = 0; i < menuCount; i++) {
            if (i == index) {
                SetConsoleTextAttribute(hConsole, 0x9F); 
                printf("  -> %s  \n", menuItems[i]);
                SetConsoleTextAttribute(hConsole, 0xF0); 
            } else {
                printf("     %s\n", menuItems[i]);
            }
        }
        printf("==========================================================\n");
        printf("使用 ↑ ↓ 方向键选择，回车确认\n");

        ch = _getch(); 
        if (ch == 0 || ch == 224) { 
            ch = _getch();
            if (ch == KEY_UP) {
                index--; 
                if (index < 0) index = menuCount - 1; 
            } else if (ch == KEY_DOWN) {
                index++;
                if (index >= menuCount) index = 0; 
            }
        } else if (ch == KEY_ENTER) {
            return index+1; 
        }
    }
}

void addRecord(Record records[], int *recordCount) {
    char choice;  
    do {
        if (*recordCount >= MAX) {
            printf("记录已满，无法新增。\n");
            return;
        }

        printf("请输入消费时间（格式：YYYY-MM-DD）：");
        scanf("%s", records[*recordCount].date);

        printf("请输入消费金额：");
        scanf("%f", &records[*recordCount].amount);
        getchar();

        printf("请输入消费类别：");
        fgets(records[*recordCount].type, sizeof(records[*recordCount].type), stdin);
        records[*recordCount].type[strcspn(records[*recordCount].type, "\n")] = '\0';

        printf("请输入备注：");
        fgets(records[*recordCount].remark, sizeof(records[*recordCount].remark), stdin);
        records[*recordCount].remark[strcspn(records[*recordCount].remark, "\n")] = '\0';

        (*recordCount)++;  
        printf("新增成功。\n");

        printf("是否继续新增记录？(y/n)：");
        scanf(" %c", &choice);  
        getchar();  
    } while (choice == 'y' || choice == 'Y');  
}
 
void deleteRecord(Record records[], int *recordCount) {
    char targetTime[20];   
    int found = 0;        
    int i, j;
 
    printf("\n==================== 当前消费记录 ====================\n");
    if (*recordCount == 0) {  
        printf("暂无任何消费记录！\n");
        return;   
    }
    for (i = 0; i < *recordCount; i++) {   
        printf("第%d条\n", i + 1);
        printf("时间：%s\n", records[i].date);
        printf("类别：%s\n", records[i].type);
        printf("金额：%.2f\n", records[i].amount);
        printf("备注：%s\n", records[i].remark);
        printf("----------------------------------------------------\n");
    }
  
    printf("\n请输入要删除的记录时间（格式：YYYY-MM-DD）：");
    fgets(targetTime, sizeof(targetTime), stdin);
    targetTime[strcspn(targetTime, "\n")] = '\0';

    for (i = 0; i < *recordCount; i++) {  
        if (strcmp(records[i].date, targetTime) == 0) {
            found = 1;
            for (j = i; j < *recordCount - 1; j++) {  
                records[j] = records[j + 1];
            }
            (*recordCount)--;  
            printf("删除成功！已移除时间为【%s】的记录\n", targetTime);
            break;  
        }
    }
    if (!found) {
        printf("未检测到时间为【%s】的消费记录\n", targetTime);
    }
}

int searchRecord(Record records[], int recordCount) {
    int choice, count = 0, i;
    char keyword[30];
    printf("\n===== 消费记录查询 =====\n");
    printf("1. 按日期查询\n2. 按类别查询\n请选择：");
    scanf("%d", &choice);
    getchar();  
    if (choice == 1) {
        printf("请输入查询日期（YYYY-MM-DD）：");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';
        printf("\n===== 查询结果 =====\n");
        for (i = 0; i < recordCount; i++) {
            if (strcmp(records[i].date, keyword) == 0) {
                printf("序号：%d | 日期：%s | 类别：%s | 金额：%.2f | 备注：%s\n",
                       i + 1, records[i].date, records[i].type, records[i].amount, records[i].remark);
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
                       i + 1, records[i].date, records[i].type, records[i].amount, records[i].remark);
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

int modifyRecord(Record records[], int recordCount) {
    char targetDate[15];
    int index = -1, i;
    printf("\n===== 消费记录修改 =====\n");
    printf("请输入要修改的记录日期（YYYY-MM-DD）：");
    scanf("%s", targetDate);
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
int isValidMonthFormat(const char *month) {
    if (strlen(month) != 7) return 0;
    if (month[4] != '-') return 0;
    int i;
    for (i = 0; i < 4; i++) {
        if (!isdigit(month[i])) return 0;
    }
    int mon = atoi(&month[5]);
    if (mon < 1 || mon > 12) return 0;
    return 1;
}

void monthlySummary(Record records[], int recordCount) {
    char month[8];
    int inputValid = 0;
 
    while (!inputValid) {
        printf("请输入要汇总的月份（格式：YYYY-MM）：");
        scanf("%7s", month);
        getchar(); 

        if (isValidMonthFormat(month)) {
            inputValid = 1;
        } else {
            printf("? 月份格式错误！请按 YYYY-MM 输入（例如2026-03），月份范围1-12\n");
        }
    }

    float sum = 0.0;     
    int recordNum = 0, i;   
 
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
        printf("?? 该月份无任何消费记录！\n");
    } else {
        float avg = sum / recordNum;  
        printf("?? 统计结果：\n");
        printf("   月度消费记录数：%d 条\n", recordNum);
        printf("   月度总消费金额：%.2f 元\n", sum);
        printf("   月度平均单笔消费：%.2f 元\n", avg);
    }
    printf("========================================================\n");
}

void printColoredBar(float ratio, int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int barLength = (int)(ratio / 100.0 * 40); 
    if (barLength < 1 && ratio > 0) barLength = 1; 
    
    SetConsoleTextAttribute(hConsole, color);
    int i;
    for (i = 0; i < barLength; i++) {
        printf("="); 
    }
    SetConsoleTextAttribute(hConsole, 0xF0); 
}

void calculateMonthlyCategoryRatio(Record records[], int recordCount) {
    char targetMonth[8];  
    int inputValid = 0;
    char categories[MAX_CATEGORY][30];   
    float catSum[MAX_CATEGORY];        
    int catCount = 0;                   
 
    while (!inputValid) {
        printf("请输入要统计的月份（格式：YYYY-MM）：");
        scanf("%7s", targetMonth);
        if (isValidMonthFormat(targetMonth)) {
            inputValid = 1;
        } else {
            printf("格式错误！请按 YYYY-MM 输入（例如2026-03），月份1-12\n");
        }
    }

    float total = 0.0;  
    int i, j;
    int isExist;
 
    catCount = 0;
    memset(categories, 0, sizeof(categories));
    memset(catSum, 0, sizeof(catSum));


    for (i = 0; i < recordCount; i++) {
        char recordMonth[8];
        strncpy(recordMonth, records[i].date, 7);   
        recordMonth[7] = '\0';
 
        if (strcmp(recordMonth, targetMonth) != 0) {
            continue;
        }

        total += records[i].amount;   
 
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
    
    {
        int colors[] = {0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xF9, 0xF2, 0xF6}; 
        int barColor;
        float ratio;
        printf("\n-------------------- 月度类别占比可视化（条形图）--------------------\n");
        for (i = 0; i < catCount; i++) {
            ratio = (catSum[i] / total) * 100;
            barColor = colors[i % 8]; 
            printf("%-10s [", categories[i]);
            printColoredBar(ratio, barColor); 
            printf("] %.1f%%\n", ratio);
        }
        printf("--------------------------------------------------------------------\n");
	}
}
 
void queryMaxMinConsumption(Record records[], int recordCount) {
 
    if (recordCount == 0) {
        printf("\n===== 最高/最低消费查询 =====\n");
        printf("暂无任何消费记录，无法查询！\n");
        return;
    }
    int maxIndex = 0, minIndex = 0, i;
    float maxAmount = records[0].amount;
    float minAmount = records[0].amount;
    for (i = 1; i < recordCount; i++) {
   
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
    printf("金额：%.2f 元\n", records[minIndex].amount);
    printf("备注：%s\n", records[minIndex].remark);
    printf("==================================\n");
}
void loadRecordsFromFile(Record records[], int *recordCount) {
    FILE *fp = fopen(DATA_FILE, "r"); 
    if (fp == NULL) {
        printf("?? 首次运行，未检测到数据文件（%s），将创建新文件\n", DATA_FILE);
        return;
    }

    *recordCount = 0;
    while (*recordCount < MAX &&
           fscanf(fp, "%14[^|]|%29[^|]|%f|%49[^\n]\n",
                  records[*recordCount].date,
                  records[*recordCount].type,
                  &records[*recordCount].amount,
                  records[*recordCount].remark) == 4) {
        (*recordCount)++;
    }

    fclose(fp);
    printf("? 成功从文件加载 %d 条消费记录\n", *recordCount);
}

void saveRecordsToFile(Record records[], int recordCount) {
    FILE *fp = fopen(DATA_FILE, "w");  
    if (fp == NULL) {
        printf("? 保存失败！无法打开文件（%s）\n", DATA_FILE);
        return;
    }

    int i;
    for (i = 0; i < recordCount; i++) {
        fprintf(fp, "%s|%s|%.2f|%s\n",
                records[i].date,
                records[i].type,
                records[i].amount,
                records[i].remark);
    }

    fclose(fp);
    printf("? 成功保存 %d 条消费记录到文件（%s）\n", recordCount, DATA_FILE);
}
 
void showMainMenu(Record records[], int *recordCount) {
    int choice;
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
    int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

    do {

        choice = selectMenu(menuItems, menuCount);
        switch (choice) {
            case 1: addRecord(records, recordCount); break;
            case 2: deleteRecord(records, recordCount); break;
            case 3: searchRecord(records, *recordCount); break;
            case 4: modifyRecord(records, *recordCount); break;
            case 5: saveRecordsToFile(records, *recordCount); break;
            case 6: monthlySummary(records, *recordCount); break;
            case 7: calculateMonthlyCategoryRatio(records, *recordCount); break;
            case 8: queryMaxMinConsumption(records, *recordCount); break;
            case 9:
                saveRecordsToFile(records, *recordCount);
                printf("系统已退出，所有记录已自动保存！\n");
                break;
            default:
                printf("输入无效，请选择1-9之间的数字！\n");
        }

        if (choice != 9) {
            printf("\n操作执行完毕，按任意键返回主菜单...");
            _getch();
        }

    } while (choice != 9);
}

int main() {
    system("color F0");
    int i;
    Record records[MAX];
    int recordCount = 0;
    loadRecordsFromFile(records, &recordCount);
    showMainMenu(records, &recordCount);
    return 0;
}
