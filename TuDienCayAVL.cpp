#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include <conio.h>
#include <ctype.h>
#include <time.h>
#include <wchar.h>

#define CR_KTK 40
#define HIEN_THI_TOI_DA 10
#define MAX_LICH_SU 18
#define MAX_CHUOI 200
#define MAX_DOAN_SAI 3
#define MAX_LENGTH 20
#define TRUE 1
#define FALSE 0
#define MAX_INPUT_LEN 50

// Các cấu trúc dữ liệu
struct NODEVIDU {
    char info[MAX_CHUOI];
    struct NODEVIDU* next;
};

struct AWORD {
    char tu[MAX_CHUOI];
    char loai[MAX_CHUOI];
    char phienam[MAX_CHUOI];
    char nghia[MAX_CHUOI];
    struct NODEVIDU* first;
};

struct NODEWORD {
    struct AWORD info;
    struct NODEWORD* left;
    struct NODEWORD* right;
    int height;
};

struct hashtable {
    struct NODEWORD* root;
};

struct NODELICHSU {
    char tu[MAX_CHUOI];
    struct NODELICHSU* next;
};

struct LENGTHNODE {
    struct NODEWORD* word;
    struct LENGTHNODE* next;
};

#define NODEVIDU struct NODEVIDU
#define AWORD struct AWORD
#define NODEWORD struct NODEWORD
#define hashtable struct hashtable
#define NODELICHSU struct NODELICHSU
#define LENGTHNODE struct LENGTHNODE

// Các biến toàn cục
char TEN_UNG_DUNG[] = "Chương Trình Học Tiếng Anh";
NODELICHSU* LichSuHead = NULL;
int SoLuongLichSu = 0;
int diemSo = 0;
LENGTHNODE* lengthLists[MAX_LENGTH + 1] = { NULL };

// Các hàm xử lý giao diện
void setUTF8Console();
void gotoxy(short x, short y);
void setColor(short textColor, short bgColor);
void setCursorVisibility(int visible);
void clearScreen();
void getConsoleSize(int* width, int* height);
void veKhung(int x, int y, int width, int height, short color, int offsetX, int offsetY);
void VeGiaoDienChinh(hashtable* tudien, char* input, int pos, int dieukien);
void VeGiaoDienChiTietTu(NODEWORD* p);

// Các hàm xử lý file
NODEWORD* ChuyenDoi(const char* dong);
int DocFile(hashtable* tudien);
void GhiAVLVaoFile(FILE* fo, NODEWORD* root);
void GhiFile(hashtable* tudien);

// Các hàm xử lý cây AVL và từ điển
int GetHeight(NODEWORD* node);
int GetBalance(NODEWORD* node);
NODEWORD* RotateRight(NODEWORD* y);
NODEWORD* RotateLeft(NODEWORD* x);
NODEWORD* BalanceNode(NODEWORD* node);
NODEWORD* ChenVaoAVL(NODEWORD* root, NODEWORD* p);
NODEWORD* TimKiemAVL(NODEWORD* root, const char* input);
NODEWORD* TimKiem(hashtable* tudien, const char* input);
NODEWORD* CapNhatDanhSachTu(hashtable* tudien, char* input, int pos, int offsetX, int offsetY);
void CapNhatDanhSachTuAVL(NODEWORD* root, const char* input, int* pos, int* count, NODEWORD** tuHienTai, int offsetX, int offsetY);
void ChenVaoLengthList(NODEWORD* word);
void ChenVdVaoDauDs(NODEVIDU** first, const char* vidu);
void ThemVaoLichSu(const char* tu);
int TapTuDien(const char* p);
void DemSoTuAVL(NODEWORD* root, int* count);
int DemSoTu(hashtable* tudien);
void DemSoTuHienThiAVL(NODEWORD* root, const char* input, int* count);
int DemSoTuHienThi(hashtable* tudien, const char* input);
NODEWORD* TimNhoNhat(NODEWORD* root);
NODEWORD* XoaTuAVL(NODEWORD* root, const char* tu);
void XoaTu(hashtable* tudien, NODEWORD** p);
void SuaTu(NODEWORD* p);
void ThemTuMoi(hashtable* tudien);
void XemLichSuTraCuu();
void XuLyTuDien(hashtable* tudien);
void GiaiPhongAVL(NODEWORD* root);
void CleanUp(hashtable* tudien);

// Các hàm xử lý trò chơi
NODEWORD* ChonTuNgauNhien(hashtable* tudien, int doKho);
void TaoTuDoChu(const char* tu, char* tuDoChu);
void ChoiTroChoiDoChu(hashtable* tudien, int* diemSo);
int GoiYKyTu(const char* tuGoc, char* tuDoChu, int len);

// Hàm phụ trợ
void catChuoiNhap(char* str, int maxLen);
int nhapChuoi(const char* prompt, char* dest, int maxLen, int offsetX, int offsetY, int y);

int main() {
    setUTF8Console();
    srand((unsigned int)time(NULL));

    hashtable* tudien = (hashtable*)calloc(26, sizeof(hashtable));
    if (tudien == NULL) {
        printf("Lỗi cấp phát bộ nhớ!\n");
        return 1;
    }
    setCursorVisibility(0);

    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    if (consoleWidth < 70 || consoleHeight < HIEN_THI_TOI_DA + 15) {
        printf("Kích thước console quá nhỏ! Cần ít nhất 70x%d ký tự.\n", HIEN_THI_TOI_DA + 15);
        free(tudien);
        return 1;
    }

    if (DocFile(tudien) == TRUE) {
        XuLyTuDien(tudien);
        GhiFile(tudien);
        CleanUp(tudien);
    } else {
        printf("Không thể mở file 'Final_Vocabulary.txt'! Vui lòng kiểm tra lại.\n");
        Sleep(2000);
    }

    free(tudien);
    setCursorVisibility(1);
    return 0;
}

// Hàm phụ trợ
void catChuoiNhap(char* str, int maxLen) {
    if (strlen(str) > maxLen) str[maxLen] = '\0';
    str[maxLen] = '\0'; // Đảm bảo kết thúc bằng null
}

int nhapChuoi(const char* prompt, char* dest, int maxLen, int offsetX, int offsetY, int y) {
    gotoxy(5 + offsetX, y + offsetY); printf("%s", prompt);
    setCursorVisibility(1);
    fgets(dest, maxLen, stdin);
    dest[strcspn(dest, "\n")] = '\0';
    setCursorVisibility(0);
    if (strcmp(dest, "0") == 0) return 0;
    catChuoiNhap(dest, maxLen - 1);
    return 1;
}

// Các hàm xử lý giao diện
void gotoxy(short x, short y) {
    HANDLE hConsoleOutput = GetStdHandle(STD_OUTPUT_HANDLE);
    COORD pos = { x, y };
    SetConsoleCursorPosition(hConsoleOutput, pos);
}

void setColor(short textColor, short bgColor) {
    HANDLE hConsoleColor = GetStdHandle(STD_OUTPUT_HANDLE);
    SetConsoleTextAttribute(hConsoleColor, (bgColor << 4) | textColor);
}

void setCursorVisibility(int visible) {
    HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_CURSOR_INFO cursorInfo;
    GetConsoleCursorInfo(hConsole, &cursorInfo);
    cursorInfo.bVisible = visible;
    SetConsoleCursorInfo(hConsole, &cursorInfo);
}

void clearScreen() {
    system("cls");
    setCursorVisibility(0);
}

void getConsoleSize(int* width, int* height) {
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(GetStdHandle(STD_OUTPUT_HANDLE), &csbi);
    *width = csbi.srWindow.Right - csbi.srWindow.Left + 1;
    *height = csbi.srWindow.Bottom - csbi.srWindow.Top + 1;
}

void setUTF8Console() {
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);
}

void veKhung(int x, int y, int width, int height, short color, int offsetX, int offsetY) {
    x += offsetX;
    y += offsetY;
    setColor(color, 0);

    gotoxy(x, y);
    printf("╔");
    for (int i = 0; i < width - 2; i++) printf("═");
    printf("╗");

    for (int i = 1; i < height - 1; i++) {
        gotoxy(x, y + i);
        printf("║");
        gotoxy(x + width - 1, y + i);
        printf("║");
    }

    gotoxy(x, y + height - 1);
    printf("╚");
    for (int i = 0; i < width - 2; i++) printf("═");
    printf("╝");
}

void ChenVdVaoDauDs(NODEVIDU** first, const char* vidu) {
    if (vidu == NULL || vidu[0] == '\0') return;
    NODEVIDU* p = (NODEVIDU*)malloc(sizeof(NODEVIDU));
    if (p == NULL) {
        printf("Lỗi cấp phát bộ nhớ!\n");
        return;
    }
    strncpy(p->info, vidu, MAX_CHUOI - 1);
    p->info[MAX_CHUOI - 1] = '\0';
    p->next = *first;
    *first = p;
}

void ThemVaoLichSu(const char* tu) {
    if (tu == NULL || tu[0] == '\0') return;

    NODELICHSU* current = LichSuHead;
    NODELICHSU* prev = NULL;
    while (current != NULL) {
        if (strcmp(current->tu, tu) == 0) {
            if (prev != NULL) {
                prev->next = current->next;
                current->next = LichSuHead;
                LichSuHead = current;
                if (SoLuongLichSu < MAX_LICH_SU) SoLuongLichSu++;
            }
            return;
        }
        prev = current;
        current = current->next;
    }

    NODELICHSU* newNode = (NODELICHSU*)malloc(sizeof(NODELICHSU));
    if (newNode == NULL) return;
    strncpy(newNode->tu, tu, MAX_CHUOI - 1);
    newNode->tu[MAX_CHUOI - 1] = '\0';
    newNode->next = NULL;

    if (SoLuongLichSu == MAX_LICH_SU) {
        NODELICHSU* last = LichSuHead;
        NODELICHSU* secondLast = NULL;
        while (last->next != NULL) {
            secondLast = last;
            last = last->next;
        }
        if (secondLast) secondLast->next = NULL;
        free(last);
        SoLuongLichSu--;
    }

    newNode->next = LichSuHead;
    LichSuHead = newNode;
    SoLuongLichSu++;
}

int TapTuDien(const char* p) {
    if (p == NULL || p[0] == '\0') return 0;
    char c = tolower(p[0]);
    return (c >= 'a' && c <= 'z') ? (c - 'a') : 0;
}

// Các hàm hỗ trợ cây AVL
int GetHeight(NODEWORD* node) {
    return node ? node->height : 0;
}

int GetBalance(NODEWORD* node) {
    return node ? GetHeight(node->left) - GetHeight(node->right) : 0;
}

int max(int a, int b) {
    return a > b ? a : b;
}

void updateHeight(NODEWORD* node) {
    if (node) node->height = 1 + max(GetHeight(node->left), GetHeight(node->right));
}

NODEWORD* RotateRight(NODEWORD* x) {
    if (!x || !x->left) return x;
    NODEWORD* y = x->left;
    NODEWORD* T2 = y->right;
    y->right = x;
    x->left = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

NODEWORD* RotateLeft(NODEWORD* x) {
    if (!x || !x->right) return x;
    NODEWORD* y = x->right;
    NODEWORD* T2 = y->left;
    y->left = x;
    x->right = T2;
    updateHeight(x);
    updateHeight(y);
    return y;
}

NODEWORD* BalanceNode(NODEWORD* node) {
    if (!node) return node;
    int balance = GetBalance(node);
    if (balance > 1) {
        if (GetBalance(node->left) < 0) {
            node->left = RotateLeft(node->left);
        }
        return RotateRight(node);
    }
    if (balance < -1) {
        if (GetBalance(node->right) > 0) {
            node->right = RotateRight(node->right);
        }
        return RotateLeft(node);
    }
    return node;
}

NODEWORD* ChenVaoAVL(NODEWORD* root, NODEWORD* p) {
    if (p == NULL) return root;
    if (root == NULL) {
        p->left = p->right = NULL;
        p->height = 1;
        return p;
    }

    int cmp = strcmp(p->info.tu, root->info.tu);
    if (cmp < 0) {
        root->left = ChenVaoAVL(root->left, p);
    } else if (cmp > 0) {
        root->right = ChenVaoAVL(root->right, p);
    } else {
        free(p);
        return root;
    }

    updateHeight(root);
    return BalanceNode(root);
}

void ChenVaoLengthList(NODEWORD* word) {
    if (word == NULL) return;
    int len = strlen(word->info.tu);
    if (len > MAX_LENGTH) len = MAX_LENGTH;
    LENGTHNODE* newNode = (LENGTHNODE*)malloc(sizeof(LENGTHNODE));
    if (newNode == NULL) return;
    newNode->word = word;
    newNode->next = lengthLists[len];
    lengthLists[len] = newNode;
}

NODEWORD* ChuyenDoi(const char* dong) {
    if (dong == NULL || dong[0] == '\0') return NULL;

    int dashCount = 0;
    for (int i = 0; dong[i]; i++) {
        if (dong[i] == '-') dashCount++;
    }
    if (dashCount != 3) return NULL;

    NODEWORD* p = (NODEWORD*)calloc(1, sizeof(NODEWORD));
    if (p == NULL) {
        printf("Lỗi cấp phát bộ nhớ!\n");
        return NULL;
    }
    p->info.first = NULL; // Khởi tạo danh sách ví dụ
    p->height = 1;

    char tu[MAX_CHUOI] = "", loai[MAX_CHUOI] = "", phienam[MAX_CHUOI] = "", nghia[MAX_CHUOI] = "";
    int i = 0, pos = 0;

    while (dong[i] && dong[i] != '-') tu[pos++] = tolower(dong[i++]);
    tu[pos] = '\0';
    if (i >= strlen(dong) || tu[0] == '\0') {
        free(p);
        return NULL;
    }
    strncpy(p->info.tu, tu, MAX_CHUOI - 1);
    i++;
    pos = 0;

    while (dong[i] && dong[i] != '-') loai[pos++] = dong[i++];
    loai[pos] = '\0';
    if (i >= strlen(dong)) {
        free(p);
        return NULL;
    }
    strncpy(p->info.loai, loai, MAX_CHUOI - 1);
    i++;
    pos = 0;

    while (dong[i] && dong[i] != '-') phienam[pos++] = dong[i++];
    phienam[pos] = '\0';
    if (i >= strlen(dong)) {
        free(p);
        return NULL;
    }
    strncpy(p->info.phienam, phienam, MAX_CHUOI - 1);
    i++;
    pos = 0;

    while (dong[i]) nghia[pos++] = dong[i++];
    nghia[pos] = '\0';
    strncpy(p->info.nghia, nghia, MAX_CHUOI - 1);

    return p;
}

int DocFile(hashtable* tudien) {
    FILE* fi = fopen("Final_Vocabulary.txt", "r");
    if (fi == NULL) {
        printf("Không thể mở file 'Final_Vocabulary.txt' để đọc!\n");
        return FALSE;
    }

    char dong[MAX_CHUOI];
    while (fgets(dong, MAX_CHUOI, fi)) {
        dong[strcspn(dong, "\n")] = '\0';
        if (dong[0] == '\0') continue;

        NODEWORD* p = ChuyenDoi(dong);
        if (p == NULL) continue;

        int viduCount = 0;
        while (viduCount < 2 && fgets(dong, MAX_CHUOI, fi)) {
            dong[strcspn(dong, "\n")] = '\0';
            if (dong[0] != '\0') {
                ChenVdVaoDauDs(&p->info.first, dong);
                viduCount++;
            }
        }

        int index = TapTuDien(p->info.tu);
        tudien[index].root = ChenVaoAVL(tudien[index].root, p);
        ChenVaoLengthList(p);
    }
    fclose(fi);
    return TRUE;
}

void GhiAVLVaoFile(FILE* fo, NODEWORD* root) {
    if (root == NULL) return;
    GhiAVLVaoFile(fo, root->left);
    fprintf(fo, "%s-%s-%s-%s\n", root->info.tu, root->info.loai, root->info.phienam, root->info.nghia);
    NODEVIDU* vd = root->info.first;
    while (vd != NULL) {
        fprintf(fo, "%s\n", vd->info);
        vd = vd->next;
    }
    GhiAVLVaoFile(fo, root->right);
}

void GhiFile(hashtable* tudien) {
    FILE* fo = fopen("Final_Vocabulary.txt", "w");
    if (fo == NULL) {
        printf("Không thể mở file 'Final_Vocabulary.txt' để ghi!\n");
        Sleep(1000);
        return;
    }
    for (int i = 0; i < 26; i++) {
        GhiAVLVaoFile(fo, tudien[i].root);
    }
    fclose(fo);
    printf("Đã lưu từ điển vào Final_Vocabulary.txt\n");
}

NODEWORD* TimKiemAVL(NODEWORD* root, const char* input) {
    if (root == NULL || input == NULL || input[0] == '\0') return NULL;
    int cmp = strcmp(input, root->info.tu);
    if (cmp == 0) return root;
    if (cmp < 0) return TimKiemAVL(root->left, input);
    return TimKiemAVL(root->right, input);
}

NODEWORD* TimKiem(hashtable* tudien, const char* input) {
    if (tudien == NULL || input == NULL) return NULL;
    int index = TapTuDien(input);
    return TimKiemAVL(tudien[index].root, input);
}

void CapNhatDanhSachTuAVL(NODEWORD* root, const char* input, int* pos, int* count, NODEWORD** tuHienTai, int offsetX, int offsetY) {
    if (root == NULL || *count >= HIEN_THI_TOI_DA) return;
    CapNhatDanhSachTuAVL(root->left, input, pos, count, tuHienTai, offsetX, offsetY);
    if (*count >= HIEN_THI_TOI_DA) return;

    if (input[0] == '\0' || strncmp(root->info.tu, input, strlen(input)) == 0) {
        if (*pos <= 0) {
            if (*count == 0) {
                *tuHienTai = root;
                setColor(12, 7);
            } else {
                setColor(15, 0);
            }
            gotoxy(4 + offsetX, 9 + offsetY + *count);
            printf("%-50s\n", root->info.tu);
            (*count)++;
        }
        (*pos)--;
    }
    CapNhatDanhSachTuAVL(root->right, input, pos, count, tuHienTai, offsetX, offsetY);
}

NODEWORD* CapNhatDanhSachTu(hashtable* tudien, char* input, int pos, int offsetX, int offsetY) {
    if (tudien == NULL) return NULL;
    gotoxy(4 + offsetX, 9 + offsetY);
    int count = 0;
    NODEWORD* tuHienTai = NULL;
    int tempPos = pos;

    for (int i = 0; i < 26 && count < HIEN_THI_TOI_DA; i++) {
        CapNhatDanhSachTuAVL(tudien[i].root, input, &tempPos, &count, &tuHienTai, offsetX, offsetY);
    }

    if (count == 0 && input[0] != '\0') {
        gotoxy(4 + offsetX, 9 + offsetY);
        setColor(12, 0);
        printf("%-60s\n", "Không tìm thấy từ!");
        setColor(15, 0);
        count++;
    }
    while (count < HIEN_THI_TOI_DA) {
        gotoxy(4 + offsetX, 9 + offsetY + count);
        setColor(15, 0);
        printf("%-50s\n", "");
        count++;
    }

    setColor(15, 0);
    setCursorVisibility(0);
    return tuHienTai;
}

void VeGiaoDienChinh(hashtable* tudien, char* input, int pos, int dieukien) {
    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);

    int offsetX = (consoleWidth - 70) / 2;
    int offsetY = (consoleHeight - (HIEN_THI_TOI_DA + 15)) / 2;

    if (dieukien == 1) {
        clearScreen();
        veKhung(0, 0, 70, 3, 11, offsetX, offsetY);
        gotoxy(20 + offsetX, 1 + offsetY); setColor(15, 11); printf(" %s ", TEN_UNG_DUNG); setColor(15, 0);

        veKhung(2, 4, 66, 3, 10, offsetX, offsetY);
        veKhung(2, 8, 66, HIEN_THI_TOI_DA + 2, 14, offsetX, offsetY);

        veKhung(0, 20, 70, 3, 13, offsetX, offsetY);
        gotoxy(1 + offsetX, 21 + offsetY); printf("Esc: Thoát | Tab: Thêm Từ | Enter: Xem Từ | 4: Trò Chơi | 5: Lịch Sử");
    }

    gotoxy(4 + offsetX, 5 + offsetY); setColor(15, 0); printf("Search: %-50s", input);
    CapNhatDanhSachTu(tudien, input, pos, offsetX, offsetY);
    setCursorVisibility(0);
}

void VeGiaoDienChiTietTu(NODEWORD* p) {
    if (p == NULL) return;
    clearScreen();
    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    int offsetX = (consoleWidth - 70) / 2;
    int offsetY = (consoleHeight - 20) / 2;

    veKhung(0, 0, 70, 20, 14, offsetX, offsetY);
    gotoxy(27 + offsetX, 1 + offsetY); setColor(14, 0); printf(" Chi Tiết Từ "); setColor(15, 0);
    gotoxy(12 + offsetX, 2 + offsetY); printf("0: Quay Lại | Tab: Sửa Từ | Backspace: Xóa Từ");

    gotoxy(2 + offsetX, 4 + offsetY); setColor(10, 0); printf("%.*s", 66, p->info.tu); setColor(15, 0);
    gotoxy(2 + offsetX, 5 + offsetY); printf("%.*s", 66, p->info.phienam);
    gotoxy(2 + offsetX, 6 + offsetY); printf("------------------------------------------------------------------");
    gotoxy(2 + offsetX, 7 + offsetY); printf("(%.*s): ", 15, p->info.loai);
    setColor(13, 0);
    int x = 19 + offsetX;
    gotoxy(x, 7 + offsetY);
    for (int i = 0; p->info.nghia[i] && x < 68 + offsetX; i++) {
        printf("%c", p->info.nghia[i]);
        x++;
    }
    setColor(15, 0);

    NODEVIDU* v = p->info.first;
    int y = 9;
    gotoxy(2 + offsetX, y + offsetY); printf("Ví dụ:");
    while (v != NULL && y < 18) {
        gotoxy(2 + offsetX, ++y + offsetY); printf("- ");
        x = 4 + offsetX;
        for (int i = 0; v->info[i] && y < 18; i++) {
            if (x >= 58 + offsetX) {
                y++;
                x = 4 + offsetX;
                gotoxy(x, y + offsetY);
            }
            printf("%c", v->info[i]);
            x++;
        }
        v = v->next;
    }

    ThemVaoLichSu(p->info.tu);
    setCursorVisibility(0);
}

void DemSoTuAVL(NODEWORD* root, int* count) {
    if (root == NULL) return;
    (*count)++;
    DemSoTuAVL(root->left, count);
    DemSoTuAVL(root->right, count);
}

int DemSoTu(hashtable* tudien) {
    if (tudien == NULL) return 0;
    int count = 0;
    for (int i = 0; i < 26; i++) {
        DemSoTuAVL(tudien[i].root, &count);
    }
    return count;
}

void DemSoTuHienThiAVL(NODEWORD* root, const char* input, int* count) {
    if (root == NULL) return;
    DemSoTuHienThiAVL(root->left, input, count);
    if (input[0] == '\0' || strncmp(root->info.tu, input, strlen(input)) == 0) {
        (*count)++;
    }
    DemSoTuHienThiAVL(root->right, input, count);
}

int DemSoTuHienThi(hashtable* tudien, const char* input) {
    if (tudien == NULL || input == NULL) return 0;
    int count = 0;
    for (int i = 0; i < 26; i++) {
        DemSoTuHienThiAVL(tudien[i].root, input, &count);
    }
    return count;
}

NODEWORD* TimNhoNhat(NODEWORD* root) {
    if (root == NULL) return NULL;
    while (root->left != NULL) root = root->left;
    return root;
}

NODEWORD* XoaTuAVL(NODEWORD* root, const char* tu) {
    if (root == NULL || tu == NULL) return NULL;

    int cmp = strcmp(tu, root->info.tu);
    if (cmp < 0) {
        root->left = XoaTuAVL(root->left, tu);
    } else if (cmp > 0) {
        root->right = XoaTuAVL(root->right, tu);
    } else {
        if (root->left == NULL) {
            NODEWORD* temp = root->right;
            NODEVIDU* vd = root->info.first;
            while (vd != NULL) {
                NODEVIDU* tempVd = vd;
                vd = vd->next;
                free(tempVd);
            }
            free(root);
            return temp;
        } else if (root->right == NULL) {
            NODEWORD* temp = root->left;
            NODEVIDU* vd = root->info.first;
            while (vd != NULL) {
                NODEVIDU* tempVd = vd;
                vd = vd->next;
                free(tempVd);
            }
            free(root);
            return temp;
        } else {
            NODEWORD* minNode = TimNhoNhat(root->right);
            root->info.first = NULL;
            NODEVIDU* vd = minNode->info.first;
            while (vd != NULL) {
                ChenVdVaoDauDs(&root->info.first, vd->info);
                vd = vd->next;
            }
            minNode->info.first = NULL;
            strcpy(root->info.tu, minNode->info.tu);
            strcpy(root->info.loai, minNode->info.loai);
            strcpy(root->info.phienam, minNode->info.phienam);
            strcpy(root->info.nghia, minNode->info.nghia);
            root->right = XoaTuAVL(root->right, minNode->info.tu);
        }
    }
    if (root == NULL) return root;
    updateHeight(root);
    return BalanceNode(root);
}

void XoaTu(hashtable* tudien, NODEWORD** p) {
    if (tudien == NULL || p == NULL || *p == NULL) return;
    clearScreen();
    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    int offsetX = (consoleWidth - 40) / 2;
    int offsetY = (consoleHeight - 5) / 2;

    veKhung(1, 1, 40, 5, 12, offsetX, offsetY);
    gotoxy(5 + offsetX, 2 + offsetY); setColor(12, 0); printf("Xoá từ '%s' (y/n)? ", (*p)->info.tu); setColor(15, 0);
    gotoxy(5 + offsetX, 3 + offsetY); printf("0: Quay Lại");
    char c = _getch();
    if (c == '0') return;
    if (c != 'y' && c != 'Y') return;

    int index = TapTuDien((*p)->info.tu);
    tudien[index].root = XoaTuAVL(tudien[index].root, (*p)->info.tu);

    int len = strlen((*p)->info.tu);
    if (len > MAX_LENGTH) len = MAX_LENGTH;
    LENGTHNODE* prev = NULL;
    LENGTHNODE* curr = lengthLists[len];
    while (curr != NULL && curr->word != *p) {
        prev = curr;
        curr = curr->next;
    }
    if (curr != NULL) {
        if (prev != NULL) prev->next = curr->next;
        else lengthLists[len] = curr->next;
        free(curr);
    }
    *p = NULL;
    gotoxy(5 + offsetX, 4 + offsetY); setColor(10, 0); printf("Đã xóa thành công !"); setColor(15, 0);
    Sleep(1000);
    setCursorVisibility(0);
}

void SuaTu(NODEWORD* p) {
    if (p == NULL) return;
    clearScreen();
    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    int offsetX = (consoleWidth - 60) / 2;
    int offsetY = (consoleHeight - 20) / 2;

    veKhung(1, 1, 60, 20, 14, offsetX, offsetY);
    gotoxy(5 + offsetX, 2 + offsetY); setColor(14, 0); printf("Sửa từ: %s", p->info.tu); setColor(15, 0);
    gotoxy(5 + offsetX, 3 + offsetY); printf("0: Quay Lại");

    char temp[MAX_CHUOI];
    sprintf(temp, "Loại từ (%s): ", p->info.loai);
    if (!nhapChuoi(temp, temp, MAX_CHUOI, offsetX, offsetY, 5)) return;
    if (temp[0] != '\0') strncpy(p->info.loai, temp, MAX_CHUOI - 1);

    sprintf(temp, "Phiên âm (%s): ", p->info.phienam);
    if (!nhapChuoi(temp, temp, MAX_CHUOI, offsetX, offsetY, 6)) return;
    if (temp[0] != '\0') strncpy(p->info.phienam, temp, MAX_CHUOI - 1);

    sprintf(temp, "Nghĩa (%s): ", p->info.nghia);
    if (!nhapChuoi(temp, temp, MAX_CHUOI, offsetX, offsetY, 7)) return;
    if (temp[0] != '\0') strncpy(p->info.nghia, temp, MAX_CHUOI - 1);

    NODEVIDU* vd = p->info.first;
    int count = 1, y = 9;
    while (vd != NULL && y < 18) {
        sprintf(temp, "Ví dụ %d (%s): ", count++, vd->info);
        if (!nhapChuoi(temp, temp, MAX_CHUOI, offsetX, offsetY, y)) return;
        if (temp[0] != '\0') strncpy(vd->info, temp, MAX_CHUOI - 1);
        vd = vd->next;
        y++;
    }

    gotoxy(5 + offsetX, y + offsetY); setColor(10, 0); printf("Đã thay đổi !"); setColor(15, 0);
    Sleep(1000);
    setCursorVisibility(0);
}

void ThemTuMoi(hashtable* tudien) {
    if (tudien == NULL) return;
    NODEWORD* p = (NODEWORD*)calloc(1, sizeof(NODEWORD));
    if (p == NULL) {
        printf("Lỗi cấp phát bộ nhớ!\n");
        Sleep(1000);
        return;
    }
    p->info.first = NULL;
    p->height = 1;

    clearScreen();
    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    int offsetX = (consoleWidth - 60) / 2;
    int offsetY = (consoleHeight - 20) / 2;

    veKhung(1, 1, 60, 20, 14, offsetX, offsetY);
    gotoxy(24 + offsetX, 2 + offsetY); setColor(14, 0); printf("Thêm Từ Mới"); setColor(15, 0);
    gotoxy(5 + offsetX, 3 + offsetY); printf("0: Quay Lại");

    char temp[MAX_CHUOI];
    if (!nhapChuoi("Nhập Từ: ", temp, MAX_CHUOI, offsetX, offsetY, 5) || temp[0] == '\0') {
        free(p);
        return;
    }
    for (int i = 0; temp[i]; i++) temp[i] = tolower(temp[i]);
    for (int i = 0; temp[i]; i++) {
        if (!isalpha(temp[i])) {
            gotoxy(5 + offsetX, 6 + offsetY); setColor(12, 0); printf("Từ chỉ được chứa chữ cái!");
            setColor(15, 0);
            Sleep(1000);
            free(p);
            return;
        }
    }
    if (TimKiem(tudien, temp)) {
        gotoxy(5 + offsetX, 6 + offsetY); setColor(12, 0); printf("Từ '%s' đã tồn tại !", temp);
        setColor(15, 0);
        Sleep(1000);
        free(p);
        return;
    }
    strncpy(p->info.tu, temp, MAX_CHUOI - 1);

    if (!nhapChuoi("Loại từ: ", temp, MAX_CHUOI, offsetX, offsetY, 7)) {
        free(p);
        return;
    }
    strncpy(p->info.loai, (temp[0] != '\0') ? temp : "Trống", MAX_CHUOI - 1);

    if (!nhapChuoi("Phiên âm: ", temp, MAX_CHUOI, offsetX, offsetY, 8)) {
        free(p);
        return;
    }
    strncpy(p->info.phienam, (temp[0] != '\0') ? temp : "Trống", MAX_CHUOI - 1);

    if (!nhapChuoi("Nghĩa: ", temp, MAX_CHUOI, offsetX, offsetY, 9)) {
        free(p);
        return;
    }
    strncpy(p->info.nghia, (temp[0] != '\0') ? temp : "Trống", MAX_CHUOI - 1);

    int count = 1, y = 11;
    while (count <= 2) {
        sprintf(temp, "Ví dụ %d: ", count++);
        if (!nhapChuoi(temp, temp, MAX_CHUOI, offsetX, offsetY, y)) {
            free(p);
            return;
        }
        if (temp[0] != '\0') ChenVdVaoDauDs(&p->info.first, temp);
        y++;
    }
    int index = TapTuDien(p->info.tu);
    tudien[index].root = ChenVaoAVL(tudien[index].root, p);
    ChenVaoLengthList(p);
    GhiFile(tudien);
    gotoxy(5 + offsetX, y + offsetY); setColor(10, 0);
    printf("Đã thêm từ '%.40s' thành công !", p->info.tu);
    setColor(15, 0);
    y++;
    gotoxy(5 + offsetX, y + offsetY); printf("Số từ trong từ điển: %d", DemSoTu(tudien));
    Sleep(1000);
    setCursorVisibility(0);
}

NODEWORD* ChonTuNgauNhien(hashtable* tudien, int doKho) {
    if (tudien == NULL) return NULL;
    int minLen = (doKho == 1) ? 3 : 5;
    int maxLen = (doKho == 1) ? 4 : 8;
    int total = 0;
    for (int len = minLen; len <= maxLen; len++) {
        LENGTHNODE* curr = lengthLists[len];
        while (curr) {
            total++;
            curr = curr->next;
        }
    }
    if (total == 0) return NULL;
    int idx = rand() % total;
    for (int len = minLen; len <= maxLen; len++) {
        LENGTHNODE* curr = lengthLists[len];
        while (curr) {
            if (idx == 0) return curr->word;
            idx--;
            curr = curr->next;
        }
    }
    return NULL;
}

void TaoTuDoChu(const char* tu, char* tuDoChu) {
    if (tu == NULL || tuDoChu == NULL) return;
    strcpy(tuDoChu, tu);
    int len = strlen(tu);
    if (len <= 2) return;

    int soKyTuThayThe = len / 2;
    if (soKyTuThayThe < 1) soKyTuThayThe = 1;

    for (int i = 0; i < soKyTuThayThe; i++) {
        int viTri;
        do {
            viTri = rand() % len;
        } while (tuDoChu[viTri] == '_');
        tuDoChu[viTri] = '_';
    }
}

int GoiYKyTu(const char* tuGoc, char* tuDoChu, int len) {
    if (tuGoc == NULL || tuDoChu == NULL || len <= 0) return 0;
    int hiddenCount = 0;
    int hiddenPositions[MAX_CHUOI];

    for (int i = 0; i < len; i++) {
        if (tuDoChu[i] == '_') {
            hiddenPositions[hiddenCount++] = i;
        }
    }
    if (hiddenCount == 0) return 0;

    int idx = rand() % hiddenCount;
    int pos = hiddenPositions[idx];
    tuDoChu[pos] = tuGoc[pos];
    return 1;
}


void ChoiTroChoiDoChu(hashtable* tudien, int* diemSo) {
    if (tudien == NULL || diemSo == NULL) return;
    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    int offsetX = (consoleWidth - 60) / 2;
    int offsetY = (consoleHeight - 20) / 2;

    clearScreen();
    veKhung(1, 1, 60, 20, 9, offsetX, offsetY);
    gotoxy(20 + offsetX, 2 + offsetY); setColor(14, 0); printf(" TRÒ CHƠI ĐOÁN CHỮ "); setColor(15, 0);
    gotoxy(5 + offsetX, 4 + offsetY); printf("  0: Quay Lại");
    gotoxy(5 + offsetX, 6 + offsetY); printf("  Chọn độ khó: 1 (Dễ) | 2 (Khó): ");
    int doKho = _getch() - '0';
    if (doKho != 1 && doKho != 2) return;

    *diemSo = 0;
    int soLanDoanSaiTong = 0;
    int gameOver = 0;

    while (!gameOver) {
        NODEWORD* tuGoc = ChonTuNgauNhien(tudien, doKho);
        if (tuGoc == NULL) {
            gotoxy(5 + offsetX, 8 + offsetY); setColor(12, 0); printf("  Không có từ phù hợp !");
            setColor(15, 0);
            gotoxy(5 + offsetX, 10 + offsetY); printf("  Nhấn phím bất kỳ để quay lại...");
            _getch();
            return;
        }

        char tuDoChu[MAX_CHUOI];
        TaoTuDoChu(tuGoc->info.tu, tuDoChu);
        char tuTam[MAX_CHUOI];
        strncpy(tuTam, tuDoChu, MAX_CHUOI - 1); tuTam[MAX_CHUOI - 1] = '\0';
        int len = strlen(tuGoc->info.tu);
        int soLanGoiY = 0;
        int hiddenCount = 0;
        for (int i = 0; i < len; i++) if (tuDoChu[i] == '_') hiddenCount++;
        int roundOver = 0;

        clearScreen();
        veKhung(1, 1, 60, 20, 9, offsetX, offsetY);
        gotoxy(20 + offsetX, 2 + offsetY); setColor(14, 0); printf(" TRÒ CHƠI ĐOÁN CHỮ "); setColor(15, 0);
        gotoxy(5 + offsetX, 4 + offsetY); printf("  Điểm: %d", *diemSo);
        gotoxy(5 + offsetX, 5 + offsetY); printf("  Lần đoán sai: %d/%d | Gợi ý còn lại: %d/1", soLanDoanSaiTong, MAX_DOAN_SAI, 1 - soLanGoiY);
        gotoxy(5 + offsetX, 7 + offsetY); printf("  Từ: %s", tuDoChu);
        gotoxy(5 + offsetX, 9 + offsetY); printf("  (1: Gợi ý | Backspace: Xóa | 0: Kết Thúc)");
        gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");

        while (!roundOver && !gameOver) {
            char KyTuNguoiChoi = _getch();

            if (KyTuNguoiChoi == '0') {
                gameOver = 1;
                break;
            }

            if (KyTuNguoiChoi == 8) { // Backspace
                int deleted = 0;
                for (int i = len - 1; i >= 0 && !deleted; i--) {
                    if (tuDoChu[i] != tuTam[i]) {
                        tuDoChu[i] = '_';
                        hiddenCount++;
                        deleted = 1;
                    }
                }
                if (!deleted) {
                    gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Không có ký tự người chơi để xóa !");
                    setColor(15, 0);
                    Sleep(1000);
                    gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");
                } else {
                    gotoxy(5 + offsetX, 7 + offsetY); printf("  Từ: %s%-40s", tuDoChu, "");
                }
                gotoxy(5 + offsetX, 9 + offsetY); printf("  (1: Gợi ý | Backspace: Xóa | 0: Kết Thúc)");
                continue;
            }

            if (KyTuNguoiChoi == '1') {
                if (soLanGoiY >= 1) {
                    gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Bạn đã hết lượt gợi ý !");
                    setColor(15, 0);
                    Sleep(1000);
                    gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");
                    continue;
                }
                if (GoiYKyTu(tuGoc->info.tu, tuDoChu, len) == 0) {
                    gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Không còn chữ cái để gợi ý !");
                    setColor(15, 0);
                    Sleep(1000);
                    gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");
                    continue;
                }
                soLanGoiY++;
                hiddenCount--;
                strncpy(tuTam, tuDoChu, MAX_CHUOI - 1); tuTam[MAX_CHUOI - 1] = '\0';
                gotoxy(5 + offsetX, 11 + offsetY); setColor(10, 0); printf("  Đã gợi ý một chữ cái !");
                setColor(15, 0);
                gotoxy(5 + offsetX, 7 + offsetY); printf("  Từ: %s", tuDoChu);
                gotoxy(5 + offsetX, 5 + offsetY); printf("  Lần đoán sai: %d/%d | Gợi ý còn lại: %d/1", soLanDoanSaiTong, MAX_DOAN_SAI, 1 - soLanGoiY);
                Sleep(1000);
                gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");
                continue;
            }

            if (KyTuNguoiChoi == 13) { // Enter
                if (strchr(tuDoChu, '_') != NULL) {
                    gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Vui lòng điền hết các ký tự!");
                    setColor(15, 0);
                    Sleep(1000);
                    gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");
                    continue;
                }
                NODEWORD* p = NULL;
                if (strcmp(tuDoChu, tuGoc->info.tu) == 0) {
                    p = tuGoc;
                } else {
                    p = TimKiem(tudien, tuDoChu);
                }
                if (p != NULL && strcmp(tuDoChu, p->info.tu) == 0) {
                    *diemSo += (doKho == 1 ? 10 : 20);
                    gotoxy(5 + offsetX, 11 + offsetY); setColor(10, 0); printf("  Chính xác! Từ '%s' thích hợp!", p->info.tu);
                    gotoxy(5 + offsetX, 12 + offsetY); printf("  + %d điểm", (doKho == 1 ? 10 : 20));
                    Sleep(1000);
                    setColor(15, 0);
                    VeGiaoDienChiTietTu(p);
                    gotoxy(5 + offsetX, 14 + offsetY); setColor(15, 0); printf("  Nhấn phím bất kỳ để tiếp tục...");
                    _getch();
                    roundOver = 1;
                    continue;
                } else {
                    soLanDoanSaiTong++;
                    if (soLanDoanSaiTong >= MAX_DOAN_SAI) {
                        gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Bạn đã thua! Từ đúng là: %s", tuGoc->info.tu);
                        Sleep(1000);
                        setColor(15, 0);
                        VeGiaoDienChiTietTu(tuGoc);
                        gotoxy(5 + offsetX, 13 + offsetY); setColor(15, 0); printf("  Nhấn phím bất kỳ để tiếp tục...");
                        _getch();
                        gameOver = 1;
                        continue;
                    } else {
                        gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Sai rồi! Từ đúng là: %s", tuGoc->info.tu);
                        Sleep(1000);
                        setColor(15, 0);
                        VeGiaoDienChiTietTu(tuGoc);
                        gotoxy(5 + offsetX, 13 + offsetY); setColor(15, 0); printf("  Nhấn phím bất kỳ để tiếp tục...");
                        _getch();
                        roundOver = 1;
                        continue;
                    }
                }
            }

            if (!isalpha(KyTuNguoiChoi)) {
                gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Vui lòng nhập chữ cái !");
                setColor(15, 0);
                Sleep(1000);
                gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");
                continue;
            }

            if (hiddenCount == 0) {
                gotoxy(5 + offsetX, 11 + offsetY); setColor(12, 0); printf("  Đã đủ ký tự !");
                setColor(15, 0);
                Sleep(500);
                gotoxy(5 + offsetX, 11 + offsetY); printf("  %-40s", "");
                continue;
            }

            int filled = 0;
			for (int i = 0; i < len && !filled; i++) {
    			if (tuDoChu[i] == '_') {
        			tuDoChu[i] = tolower(KyTuNguoiChoi);
        			hiddenCount--;
        			filled = 1;
   				}
			}
            gotoxy(5 + offsetX, 7 + offsetY); printf("  Từ: %s%-40s", tuDoChu, "");
            gotoxy(5 + offsetX, 9 + offsetY); printf("  (1: Gợi ý | Backspace: Xóa | 0: Kết Thúc)");
        }
    }

    clearScreen();
    veKhung(1, 1, 60, 20, 9, offsetX, offsetY);
    gotoxy(20 + offsetX, 2 + offsetY); setColor(14, 0); printf(" KẾT THÚC TRÒ CHƠI "); setColor(15, 0);
    gotoxy(5 + offsetX, 5 + offsetY); printf("  Điểm cuối cùng: %d", *diemSo);
    gotoxy(5 + offsetX, 7 + offsetY); printf("  Xếp hạng: ");

    if (*diemSo <= 50) {
        setColor(12, 0); printf("Beginner");
    } else if (*diemSo <= 100) {
        setColor(14, 0); printf("Intermediate");
    } else if (*diemSo <= 200) {
        setColor(10, 0); printf("Advanced");
    } else {
        setColor(11, 0); printf("Expert");
    }
    setColor(15, 0);

    gotoxy(5 + offsetX, 9 + offsetY); printf("  Nhấn phím bất kỳ để tiếp tục...");
    _getch();
    setCursorVisibility(0);
}

void XemLichSuTraCuu() {
    clearScreen();
    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    int offsetX = (consoleWidth - 40) / 2;
    int offsetY = (consoleHeight - (MAX_LICH_SU + 5)) / 2;

    veKhung(1, 1, 40, MAX_LICH_SU + 5, 14, offsetX, offsetY);
    gotoxy(12 + offsetX, 2 + offsetY); setColor(14, 0); printf("LỊCH SỬ TÌM KIẾM"); setColor(15, 0);

    if (SoLuongLichSu == 0) {
        gotoxy(5 + offsetX, 5 + offsetY); printf("Lịch sử trống.");
        gotoxy(5 + offsetX, MAX_LICH_SU + 3 + offsetY); printf("Nhấn phím bất kỳ để tiếp tục...");
        _getch();
        return;
    }

    NODELICHSU* current = LichSuHead;
    int index = 1;
    while (current != NULL && index <= MAX_LICH_SU) {
        gotoxy(5 + offsetX, 3 + index + offsetY);
        printf("%2d. %.*s", index, 30, current->tu);
        current = current->next;
        index++;
    }

    gotoxy(5 + offsetX, MAX_LICH_SU + 3 + offsetY); printf("Nhấn phím bất kỳ để tiếp tục...");
    _getch();
    setCursorVisibility(0);
}

void XuLyTuDien(hashtable* tudien) {
    if (tudien == NULL) return;
    NODEWORD* tuHienTai = NULL;
    char input[MAX_CHUOI] = "";
    int pos = 0;

    int consoleWidth, consoleHeight;
    getConsoleSize(&consoleWidth, &consoleHeight);
    int offsetX = (consoleWidth - 70) / 2;
    int offsetY = (consoleHeight - (HIEN_THI_TOI_DA + 15)) / 2;

    VeGiaoDienChinh(tudien, input, pos, 1);
    while (1) {
        tuHienTai = CapNhatDanhSachTu(tudien, input, pos, offsetX, offsetY);
        int soLuongTuHienThi = DemSoTuHienThi(tudien, input);
        int keyCode = _getch();

        switch (keyCode) {
        case 8: // Backspace
            if (strlen(input) > 0) {
                input[strlen(input) - 1] = '\0';
                pos = 0;
                gotoxy(4 + offsetX, 5 + offsetY);
                printf("Search: %-50s", input);
            }
            break;
        case 9: // Tab
            ThemTuMoi(tudien);
            VeGiaoDienChinh(tudien, input, pos, 1);
            break;
        case 13: // Enter
            if (tuHienTai) {
                VeGiaoDienChiTietTu(tuHienTai);
                while (1) {
                    int k = _getch();
                    if (k == '0') break;
                    if (k == 8) {
                        XoaTu(tudien, &tuHienTai);
                        break;
                    }
                    if (k == 9) {
                        SuaTu(tuHienTai);
                        break;
                    }
                }
                input[0] = '\0';
                pos = 0;
                VeGiaoDienChinh(tudien, input, pos, 1);
            }
            break;
        case 27: // Esc
            gotoxy(2 + offsetX, 24 + offsetY); setColor(14, 0); printf("Đang lưu..."); setColor(15, 0);
            GhiFile(tudien);
            Sleep(1000);
            gotoxy(2 + offsetX, 24 + offsetY); setColor(10, 0);
            printf("%s", "Đã lưu thành công!"); setColor(15, 0);
            return;
        case 224: // Arrow keys
            switch (_getch()) {
            case 72: if (pos > 0) pos--; break; // Up
            case 80: if (pos < soLuongTuHienThi - 1 && tuHienTai != NULL) pos++; break; // Down
            }
            break;
        case '4':
            ChoiTroChoiDoChu(tudien, &diemSo);
            VeGiaoDienChinh(tudien, input, pos, 1);
            break;
        case '5':
            XemLichSuTraCuu();
            VeGiaoDienChinh(tudien, input, pos, 1);
            break;
        default:
            if (isalpha(keyCode) && strlen(input) < CR_KTK - 10) {
                char c = tolower(keyCode);
                strncat(input, &c, 1);
                pos = 0;
                gotoxy(4 + offsetX, 5 + offsetY);
                printf("Search: %-50s", input);
            }
            break;
        }
        VeGiaoDienChinh(tudien, input, pos, 0);
    }
}

void GiaiPhongAVL(NODEWORD* root) {
    if (root == NULL) return;
    GiaiPhongAVL(root->left);
    GiaiPhongAVL(root->right);
    NODEVIDU* vd = root->info.first;
    while (vd != NULL) {
        NODEVIDU* temp = vd;
        vd = vd->next;
        free(temp);
    }
    root->info.first = NULL;
    free(root);
}

void CleanUp(hashtable* tudien) {
    if (tudien == NULL) return;
    for (int i = 0; i < 26; i++) {
        GiaiPhongAVL(tudien[i].root);
        tudien[i].root = NULL;
    }
    NODELICHSU* current = LichSuHead;
    while (current != NULL) {
        NODELICHSU* temp = current;
        current = current->next;
        free(temp);
    }
    LichSuHead = NULL;
    SoLuongLichSu = 0;

    for (int i = 0; i <= MAX_LENGTH; i++) {
        LENGTHNODE* curr = lengthLists[i];
        while (curr != NULL) {
            LENGTHNODE* temp = curr;
            curr = curr->next;
            temp->word = NULL;
            free(temp);
        }
        lengthLists[i] = NULL;
    }
}
