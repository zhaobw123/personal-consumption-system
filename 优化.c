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

/* === 新增：预算数据文件 === */
#define BUDGET_FILE "budget_records.txt"
/* === 新增结束 === */

typedef struct {
    char date[15];    
    char type[30];    
    float amount;     
    char remark[50];  
} Record;

/* === 新增：预算结构体 === */
typedef struct {
    char month[8];      /* 格式：YYYY-MM */
    char type[30];      /* 消费类别 */
    float budget;       /* 预算金额 */
} Budget;
/* === 新增结束 === */

/* === 新增：前置声明 === */
int isValidMonthFormat(const char *month);
/* === 新增结束 === */
 
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

/* === 新增：辅助函数 - 设置控制台颜色 === */
void setColor(int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsole, color);
}
/* === 新增结束 === */

/* === 新增：辅助函数 - 打印彩色提示信息 === */
void printInfo(const char *msg) {
    setColor(0xF1);
    printf("[提示] %s\n", msg);
    setColor(0xF0);
}

void printSuccess(const char *msg) {
    setColor(0xF2);
    printf("[成功] %s\n", msg);
    setColor(0xF0);
}

void printWarning(const char *msg) {
    setColor(0xF4);
    printf("[警告] %s\n", msg);
    setColor(0xF0);
}

void printError(const char *msg) {
    setColor(0xFC);
    printf("[错误] %s\n", msg);
    setColor(0xF0);
}
/* === 新增结束 === */

/* === 新增：辅助函数 - 打印表格分隔线 === */
void printTableLine(int width) {
    int i;
    for (i = 0; i < width; i++) {
        printf("-");
    }
    printf("\n");
}
/* === 新增结束 === */

/* === 新增：辅助函数 - 判断字符串包含（模糊匹配） === */
int containsSubstring(const char *str, const char *sub) {
    return strstr(str, sub) != NULL;
}
/* === 新增结束 === */

/* === 新增：辅助函数 - 获取当前月份从日期中 === */
void getMonthFromDate(const char *date, char *month) {
    strncpy(month, date, 7);
    month[7] = '\0';
}
/* === 新增结束 === */

/* === 新增：预算管理 - 加载预算 === */
void loadBudgets(Budget budgets[], int *budgetCount) {
    FILE *fp = fopen(BUDGET_FILE, "r");
    if (fp == NULL) {
        *budgetCount = 0;
        return;
    }
    *budgetCount = 0;
    while (*budgetCount < MAX &&
           fscanf(fp, "%7[^|]|%29[^|]|%f\n",
                  budgets[*budgetCount].month,
                  budgets[*budgetCount].type,
                  &budgets[*budgetCount].budget) == 3) {
        (*budgetCount)++;
    }
    fclose(fp);
}
/* === 新增结束 === */

/* === 新增：预算管理 - 保存预算 === */
void saveBudgets(Budget budgets[], int budgetCount) {
    FILE *fp = fopen(BUDGET_FILE, "w");
    if (fp == NULL) {
        printError("保存预算失败！无法打开文件。");
        return;
    }
    int i;
    for (i = 0; i < budgetCount; i++) {
        fprintf(fp, "%s|%s|%.2f\n",
                budgets[i].month, budgets[i].type, budgets[i].budget);
    }
    fclose(fp);
}
/* === 新增结束 === */

/* === 新增：预算管理 - 查找预算 === */
int findBudget(Budget budgets[], int budgetCount, const char *month, const char *type) {
    int i;
    for (i = 0; i < budgetCount; i++) {
        if (strcmp(budgets[i].month, month) == 0 && strcmp(budgets[i].type, type) == 0) {
            return i;
        }
    }
    return -1;
}
/* === 新增结束 === */

/* === 新增：预算管理 - 设置/修改/删除预算（三合一） === */
void setBudget(Budget budgets[], int *budgetCount) {
    Budget newBudget;
    int idx, i;
    int readOk;
    
    system("cls");
    printf("==================== 设置月度类别预算 ====================\n");
    
    if (*budgetCount > 0) {
        printf("\n当前已设置的预算：\n");
        printf("%-12s %-20s %-12s\n", "月份", "类别", "预算金额(元)");
        printTableLine(50);
        for (i = 0; i < *budgetCount; i++) {
            printf("%-12s %-20s %-12.2f\n", 
                   budgets[i].month, budgets[i].type, budgets[i].budget);
        }
        printTableLine(50);
        printf("\n");
    } else {
        printf("\n暂无已设置的预算\n\n");
    }
    
    printf("请输入月份（格式：YYYY-MM，输入0返回）：");
    scanf("%7s", newBudget.month);
    getchar();
    
    if (strcmp(newBudget.month, "0") == 0) {
        printInfo("已取消");
        return;
    }
    if (!isValidMonthFormat(newBudget.month)) {
        printError("月份格式错误！");
        return;
    }
    
    printf("请输入消费类别：");
    fgets(newBudget.type, sizeof(newBudget.type), stdin);
    newBudget.type[strcspn(newBudget.type, "\n")] = '\0';
    
    printf("请输入预算金额（元，输入0删除该预算）：");
    readOk = scanf("%f", &newBudget.budget);
    getchar();
    
    if (readOk != 1) {
        printError("输入无效！请输入数字。");
        while (getchar() != '\n');
        return;
    }
    
    idx = findBudget(budgets, *budgetCount, newBudget.month, newBudget.type);
    
    if (newBudget.budget <= 0) {
        if (idx >= 0) {
            for (i = idx; i < *budgetCount - 1; i++) {
                budgets[i] = budgets[i + 1];
            }
            (*budgetCount)--;
            saveBudgets(budgets, *budgetCount);
            printSuccess("预算已删除");
        } else {
            printWarning("该预算不存在，无需删除");
        }
        return;
    }
    
    if (idx >= 0) {
        budgets[idx].budget = newBudget.budget;
    } else {
        if (*budgetCount >= MAX) {
            printError("预算记录已满！");
            return;
        }
        budgets[*budgetCount] = newBudget;
        (*budgetCount)++;
    }
    saveBudgets(budgets, *budgetCount);
    printSuccess("预算设置成功！");
}
/* === 新增结束 === */

/* === 新增：预算管理 - 超支提醒 === */
void checkBudgetAlert(Record records[], int recordCount, Budget budgets[], int budgetCount, const char *month, const char *type) {
    int i, idx;
    float spent = 0.0;
    
    idx = findBudget(budgets, budgetCount, month, type);
    if (idx < 0) return;
    
    for (i = 0; i < recordCount; i++) {
        char recordMonth[8];
        getMonthFromDate(records[i].date, recordMonth);
        if (strcmp(recordMonth, month) == 0 && strcmp(records[i].type, type) == 0) {
            spent += records[i].amount;
        }
    }
    
    printf("\n");
    printTableLine(60);
    printf("【预算监控】%s | %s\n", month, type);
    printf("预算：%.2f元 | 已用：%.2f元 | 剩余：%.2f元\n", 
           budgets[idx].budget, spent, budgets[idx].budget - spent);
    
    if (spent > budgets[idx].budget) {
        printf(">>> 已超支 %.2f 元！请注意控制消费！\n", spent - budgets[idx].budget);
    } else if (spent > budgets[idx].budget * 0.8) {
        printf(">>> 已使用 %.1f%%，接近预算上限\n", (spent / budgets[idx].budget) * 100);
    } else {
        printf(">>> 预算充足，还可消费 %.2f 元\n", budgets[idx].budget - spent);
    }
    printTableLine(60);
}
/* === 新增结束 === */

/* === 修改：增加预算参数和金额校验 === */
void addRecord(Record records[], int *recordCount, Budget budgets[], int budgetCount) {
    char choice;  
    char currentMonth[8];
    
    do {
        if (*recordCount >= MAX) {
            printError("记录已满，无法新增。");
            return;
        }

        printf("请输入消费时间：");
        scanf("%s", records[*recordCount].date);

        printf("请输入消费金额：");
        scanf("%f", &records[*recordCount].amount);
        getchar();

        if (records[*recordCount].amount <= 0) {
            printError("消费金额必须大于0！");
            continue;
        }

        printf("请输入消费类别：");
        fgets(records[*recordCount].type, sizeof(records[*recordCount].type), stdin);
        records[*recordCount].type[strcspn(records[*recordCount].type, "\n")] = '\0';

        printf("请输入备注：");
        fgets(records[*recordCount].remark, sizeof(records[*recordCount].remark), stdin);
        records[*recordCount].remark[strcspn(records[*recordCount].remark, "\n")] = '\0';

        (*recordCount)++;  
        printSuccess("新增成功。");

        /* === 新增：添加记录后检查预算 === */
        getMonthFromDate(records[*recordCount - 1].date, currentMonth);
        checkBudgetAlert(records, *recordCount, budgets, budgetCount, 
                        currentMonth, records[*recordCount - 1].type);
        /* === 新增结束 === */

        printf("是否继续新增记录？(y/n)：");
        scanf(" %c", &choice);  
        getchar();  
    } while (choice == 'y' || choice == 'Y');  
}
/* === 修改结束 === */
 
/* === 修改：美化界面，支持序号删除 === */
void deleteRecord(Record records[], int *recordCount) {
    int targetIndex;
    int i, j;
 
    system("cls");
    printf("==================== 删除消费记录 ====================\n");
    if (*recordCount == 0) {  
        printWarning("暂无任何消费记录！");
        return;   
    }
    
    printf("%-4s %-12s %-15s %-10s %-20s\n", "序号", "日期", "类别", "金额(元)", "备注");
    printTableLine(70);
    for (i = 0; i < *recordCount; i++) {   
        printf("%-4d %-12s %-15s %-10.2f %-20s\n", 
               i + 1, records[i].date, records[i].type, 
               records[i].amount, records[i].remark);
    }
    printTableLine(70);
  
    printf("\n请输入要删除的记录序号（1-%d，输入0取消）：", *recordCount);
    scanf("%d", &targetIndex);
    getchar();

    if (targetIndex == 0) {
        printInfo("已取消删除操作");
        return;
    }
    
    if (targetIndex < 1 || targetIndex > *recordCount) {
        printError("序号无效！");
        return;
    }

    for (j = targetIndex - 1; j < *recordCount - 1; j++) {  
        records[j] = records[j + 1];
    }
    (*recordCount)--;  
    printSuccess("删除成功！");
}
/* === 修改结束 === */

/* === 修改：增加模糊查询和月份查询，美化界面 === */
int searchRecord(Record records[], int recordCount) {
    int choice, count = 0, i;
    char keyword[30];
    char month[8];
    
    system("cls");
    printf("==================== 消费记录查询 ====================\n");
    printf("1. 按日期查询\n");
    printf("2. 按类别查询（支持模糊）\n");
    printf("3. 按月份查询\n");
    printf("4. 按金额范围查询\n");
    printf("请选择：");
    scanf("%d", &choice);
    getchar();  
    
    if (choice == 1) {
        printf("请输入查询日期（YYYY-MM-DD）：");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';
        
        system("cls");
        printf("==================== 查询结果 ====================\n");
        printf("%-4s %-12s %-15s %-10s %-20s\n", "序号", "日期", "类别", "金额(元)", "备注");
        printTableLine(70);
        
        for (i = 0; i < recordCount; i++) {
            if (strcmp(records[i].date, keyword) == 0) {
                printf("%-4d %-12s %-15s %-10.2f %-20s\n",
                       i + 1, records[i].date, records[i].type, 
                       records[i].amount, records[i].remark);
                count++;
            }
        }
    } else if (choice == 2) {
        /* === 修改：改为模糊查询 === */
        printf("请输入查询关键词（类别包含该词即可）：");
        fgets(keyword, sizeof(keyword), stdin);
        keyword[strcspn(keyword, "\n")] = '\0';
        
        system("cls");
        printf("==================== 查询结果 ====================\n");
        printf("%-4s %-12s %-15s %-10s %-20s\n", "序号", "日期", "类别", "金额(元)", "备注");
        printTableLine(70);
        
        for (i = 0; i < recordCount; i++) {
            if (containsSubstring(records[i].type, keyword)) {
                printf("%-4d %-12s %-15s %-10.2f %-20s\n",
                       i + 1, records[i].date, records[i].type, 
                       records[i].amount, records[i].remark);
                count++;
            }
        }
        /* === 修改结束 === */
    } else if (choice == 3) {
        /* === 新增：按月份查询 === */
        printf("请输入查询月份（YYYY-MM）：");
        scanf("%7s", month);
        getchar();
        
        if (!isValidMonthFormat(month)) {
            printError("月份格式错误！");
            return 0;
        }
        
        system("cls");
        printf("==================== %s 月份查询结果 ====================\n", month);
        printf("%-4s %-12s %-15s %-10s %-20s\n", "序号", "日期", "类别", "金额(元)", "备注");
        printTableLine(70);
        
        for (i = 0; i < recordCount; i++) {
            char recordMonth[8];
            getMonthFromDate(records[i].date, recordMonth);
            if (strcmp(recordMonth, month) == 0) {
                printf("%-4d %-12s %-15s %-10.2f %-20s\n",
                       i + 1, records[i].date, records[i].type, 
                       records[i].amount, records[i].remark);
                count++;
            }
        }
        /* === 新增结束 === */
    } else if (choice == 4) {
        /* === 新增：按金额范围查询 === */
        float minAmount, maxAmount;
        printf("请输入最低金额：");
        scanf("%f", &minAmount);
        printf("请输入最高金额：");
        scanf("%f", &maxAmount);
        getchar();
        
        if (minAmount > maxAmount) {
            printError("最低金额不能大于最高金额！");
            return 0;
        }
        
        system("cls");
        printf("==================== 金额范围查询结果 ====================\n");
        printf("范围：%.2f 元 ~ %.2f 元\n", minAmount, maxAmount);
        printf("%-4s %-12s %-15s %-10s %-20s\n", "序号", "日期", "类别", "金额(元)", "备注");
        printTableLine(70);
        
        for (i = 0; i < recordCount; i++) {
            if (records[i].amount >= minAmount && records[i].amount <= maxAmount) {
                printf("%-4d %-12s %-15s %-10.2f %-20s\n",
                       i + 1, records[i].date, records[i].type, 
                       records[i].amount, records[i].remark);
                count++;
            }
        }
        /* === 新增结束 === */
    } else {
        printError("无效选择！");
        return 0;
    }
    
    printTableLine(70);
    if (count == 0) {
        printWarning("未找到匹配的消费记录！");
    } else {
        printf("共查询到 %d 条匹配记录\n", count);
    }
    return count;
}
/* === 修改结束 === */

/* === 修改：支持同一天多条记录选择，增加取消选项 === */
int modifyRecord(Record records[], int recordCount) {
    char targetDate[15];
    int indices[MAX];
    int matchCount = 0;
    int choice, index, i;
    int modifyChoice;
    
    system("cls");
    printf("==================== 修改消费记录 ====================\n");
    printf("请输入要修改的记录日期（YYYY-MM-DD）：");
    scanf("%s", targetDate);
    getchar();
    
    for (i = 0; i < recordCount; i++) {
        if (strcmp(records[i].date, targetDate) == 0) {
            indices[matchCount] = i;
            matchCount++;
        }
    }
    
    if (matchCount == 0) {
        printError("未找到该日期的消费记录！");
        return 0;
    }
    
    printf("\n找到 %d 条记录：\n", matchCount);
    printf("%-4s %-12s %-15s %-10s %-20s\n", "序号", "日期", "类别", "金额(元)", "备注");
    printTableLine(70);
    for (i = 0; i < matchCount; i++) {
        printf("%-4d %-12s %-15s %-10.2f %-20s\n",
               i + 1, records[indices[i]].date, records[indices[i]].type,
               records[indices[i]].amount, records[indices[i]].remark);
    }
    printTableLine(70);
    
    printf("\n请选择要修改的记录序号（1-%d，输入0取消）：", matchCount);
    scanf("%d", &choice);
    getchar();
    
    if (choice == 0) {
        printInfo("已取消修改操作");
        return 0;
    }
    if (choice < 1 || choice > matchCount) {
        printError("序号无效！");
        return 0;
    }
    
    index = indices[choice - 1];
    
    printf("\n当前记录：\n");
    printf("日期：%s | 类别：%s | 金额：%.2f | 备注：%s\n",
           records[index].date, records[index].type, records[index].amount, records[index].remark);
    printf("请选择修改项：1.类别 2.金额 3.备注 4.全部修改 0.取消\n请选择：");
    scanf("%d", &modifyChoice);
    getchar();
    
    if (modifyChoice == 0) {
        printInfo("已取消修改");
        return 0;
    }
    
    switch (modifyChoice) {
        case 1:
            printf("输入新类别：");
            fgets(records[index].type, sizeof(records[index].type), stdin);
            records[index].type[strcspn(records[index].type, "\n")] = '\0';
            break;
        case 2:
            printf("输入新金额：");
            scanf("%f", &records[index].amount);
            getchar();
            if (records[index].amount <= 0) {
                printError("金额必须大于0！修改失败");
                return 0;
            }
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
            if (records[index].amount <= 0) {
                printError("金额必须大于0！修改失败");
                return 0;
            }
            printf("新备注：");
            fgets(records[index].remark, sizeof(records[index].remark), stdin);
            records[index].remark[strcspn(records[index].remark, "\n")] = '\0';
            break;
        default:
            printError("无效选择，修改失败！");
            return 0;
    }
    printSuccess("记录修改成功！");
    return 1;
}
/* === 修改结束 === */

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

/* === 修改：美化界面 === */
void monthlySummary(Record records[], int recordCount) {
    char month[8];
    int inputValid = 0;
 
    system("cls");
    printf("==================== 月度消费汇总 ====================\n");
    
    while (!inputValid) {
        printf("请输入要汇总的月份（格式：YYYY-MM）：");
        scanf("%7s", month);
        getchar(); 

        if (isValidMonthFormat(month)) {
            inputValid = 1;
        } else {
            printError("月份格式错误！请按 YYYY-MM 输入（例如2026-03），月份范围1-12");
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
    
    printf("\n");
    printTableLine(60);
    printf("              %s 月度消费汇总\n", month);
    printTableLine(60);
    if (recordNum == 0) {
        printWarning("该月份无任何消费记录！");
    } else {
        float avg = sum / recordNum;  
        printf("  月度消费记录数：%d 条\n", recordNum);
        printf("  月度总消费金额：%.2f 元\n", sum);
        printf("  月度平均单笔消费：%.2f 元\n", avg);
    }
    printTableLine(60);
}
/* === 修改结束 === */

/* ==================== 新增函数：可视化辅助函数 ==================== */
/* 功能：根据占比绘制彩色条形图，使用Windows控制台API设置颜色 */
void printColoredBar(float ratio, int color) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    int barLength = (int)(ratio / 100.0 * 40); /* 条形图最大宽度为40个字符 */
    if (barLength < 1 && ratio > 0) barLength = 1; /* 占比再小也至少显示1个字符 */
    
    SetConsoleTextAttribute(hConsole, color);
    int i;
    for (i = 0; i < barLength; i++) {
        printf("="); /* 使用'='组成条形 */
    }
    SetConsoleTextAttribute(hConsole, 0xF0); /* 恢复默认颜色（白底黑字） */
}
/* ==================== 新增函数结束 ==================== */

/* === 修改：美化界面 === */
void calculateMonthlyCategoryRatio(Record records[], int recordCount) {
    char targetMonth[8];  
    int inputValid = 0;
    char categories[MAX_CATEGORY][30];   
    float catSum[MAX_CATEGORY];        
    int catCount = 0;                   
 
    system("cls");
    printf("==================== 月度类别占比统计 ====================\n");
    
    while (!inputValid) {
        printf("请输入要统计的月份（格式：YYYY-MM）：");
        scanf("%7s", targetMonth);
        if (isValidMonthFormat(targetMonth)) {
            inputValid = 1;
        } else {
            printError("格式错误！请按 YYYY-MM 输入（例如2026-03），月份1-12");
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
            printWarning("类别数量已达上限，无法统计");
        }
    }
 
    printf("\n");
    printTableLine(60);
    printf("           %s 月度类别占比统计\n", targetMonth);
    printTableLine(60);
    
    if (total == 0.0 || catCount == 0) {
        printWarning("该月无消费记录，无法统计类别占比！");
        printTableLine(60);
        return;
    }

    printf("%-10s %-12s %-8s\n", "类别", "类别总金额(元)", "占比(%)");
    printTableLine(50);
    
	
	for (i = 0; i < catCount; i++) {
        float ratio = (catSum[i] / total) * 100;
        printf("%-10s %-12.2f %-8.1f\n", categories[i], catSum[i], ratio);
    }
    printTableLine(50);
    printf("%-10s %-12.2f %-8.1f\n", "总计", total, 100.0);
    printTableLine(60);
    
    /* ==================== 修改开始：新增可视化条形图输出 ==================== */
    {
        int colors[] = {0xFA, 0xFB, 0xFC, 0xFD, 0xFE, 0xF9, 0xF2, 0xF6}; /* 白底彩色字颜色数组 */
        int barColor;
        float ratio;
        printf("\n-------------------- 月度类别占比可视化（条形图）--------------------\n");
        for (i = 0; i < catCount; i++) {
            ratio = (catSum[i] / total) * 100;
            barColor = colors[i % 8]; /* 循环使用不同颜色区分各类别 */
            printf("%-10s [", categories[i]);
            printColoredBar(ratio, barColor); /* 调用新增函数绘制彩色条形 */
            printf("] %.1f%%\n", ratio);
        }
        printf("--------------------------------------------------------------------\n");
    }
    /* ==================== 修改结束 ==================== */
}
/* === 修改结束 === */
 
/* === 修改：美化界面 === */
void queryMaxMinConsumption(Record records[], int recordCount) {
 
    if (recordCount == 0) {
        system("cls");
        printf("==================== 最高/最低消费查询 ====================\n");
        printWarning("暂无任何消费记录，无法查询！");
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
    
    system("cls");
    printf("==================== 最高/最低消费查询结果 ====================\n");
 
    setColor(0xF4);
    printf("\n【最高消费记录】\n");
    setColor(0xF0);
    printf("  日期：%s\n", records[maxIndex].date);
    printf("  类别：%s\n", records[maxIndex].type);
    printf("  金额：%.2f 元\n", records[maxIndex].amount);
    printf("  备注：%s\n", records[maxIndex].remark);
    printTableLine(50);
 
    setColor(0xF2);
    printf("\n【最低消费记录】\n");
    setColor(0xF0);
    printf("  日期：%s\n", records[minIndex].date);
    printf("  类别：%s\n", records[minIndex].type);
    printf("  金额：%.2f 元\n", records[minIndex].amount);
    printf("  备注：%s\n", records[minIndex].remark);
    printTableLine(50);
}
/* === 修改结束 === */

/* === 修改：美化提示 === */
void loadRecordsFromFile(Record records[], int *recordCount) {
    FILE *fp = fopen(DATA_FILE, "r"); 
    if (fp == NULL) {
        printInfo("首次运行，未检测到数据文件，将创建新文件");
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
    printf("成功从文件加载 %d 条消费记录\n", *recordCount);
}
/* === 修改结束 === */

/* === 修改：美化提示 === */
void saveRecordsToFile(Record records[], int recordCount) {
    FILE *fp = fopen(DATA_FILE, "w");  
    if (fp == NULL) {
        printError("保存失败！无法打开文件");
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
    printSuccess("记录已保存");
}
/* === 修改结束 === */
 
/* === 修改：增加预算管理入口和参数传递 === */
void showMainMenu(Record records[], int *recordCount, Budget budgets[], int *budgetCount) {
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
        "9. 预算管理",           /* === 新增 === */
        "10. 退出系统"           /* === 修改：序号调整 === */
    };
    int menuCount = sizeof(menuItems) / sizeof(menuItems[0]);

    do {

        choice = selectMenu(menuItems, menuCount);
        switch (choice) {
            case 1: addRecord(records, recordCount, budgets, *budgetCount); break;  /* === 修改：传递预算参数 === */
            case 2: deleteRecord(records, recordCount); break;
            case 3: searchRecord(records, *recordCount); break;
            case 4: modifyRecord(records, *recordCount); break;
            case 5: saveRecordsToFile(records, *recordCount); break;
            case 6: monthlySummary(records, *recordCount); break;
            case 7: calculateMonthlyCategoryRatio(records, *recordCount); break;
            case 8: queryMaxMinConsumption(records, *recordCount); break;
            case 9: setBudget(budgets, budgetCount); break;  /* === 新增 === */
            case 10:  /* === 修改：序号调整 === */
                saveRecordsToFile(records, *recordCount);
                printSuccess("系统已退出，所有记录已自动保存！");
                break;
            default:
                printError("输入无效，请重新选择！");
        }

        if (choice != 10) {  /* === 修改：序号调整 === */
            printf("\n操作执行完毕，按任意键返回主菜单...");
            _getch();
        }

    } while (choice != 10);  /* === 修改：序号调整 === */
}
/* === 修改结束 === */

/* === 修改：增加预算加载 === */
int main() {
    system("color F0");
    int i;
    Record records[MAX];
    int recordCount = 0;
    Budget budgets[MAX];      /* === 新增 === */
    int budgetCount = 0;      /* === 新增 === */
    
    loadRecordsFromFile(records, &recordCount);
    loadBudgets(budgets, &budgetCount);  /* === 新增 === */
    
    showMainMenu(records, &recordCount, budgets, &budgetCount);  /* === 修改：传递预算参数 === */
    return 0;
}
/* === 修改结束 === */
