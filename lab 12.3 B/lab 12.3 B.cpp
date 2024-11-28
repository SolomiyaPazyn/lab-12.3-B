#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <iomanip>
using namespace std;

// ������ ��������������
enum Spec { CS = 1, IT, ME, PhIT, TR };

// ��������� ��� ��������� ���������� ��� ��������
#pragma pack(push, 1)
struct Student {
    char lname[50];
    int kurs;
    Spec spec;
    int physics;
    int math;
    int thirdMark; // ������ ��'������� ������������� ������ int

    // ����� ��� ���������� ����� ������������
    string return_spec() {
        switch (spec) {
        case CS: return "����'�����i �����";
        case IT: return "I����������";
        case ME: return "���������� �� ������i��";
        case PhIT: return "�i���� �� i����������";
        case TR: return "������� ��������";
        default: return "�������";
        }
    }
};
#pragma pack(pop)

// ��������� �� ���������� ����� � ������� ����
void Create(const char* filename, int amSt) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "������� �������� ����� ��� ������" << endl;
        return;
    }

    for (int i = 0; i < amSt; i++) {
        Student s;
        cout << "������� �" << i + 1 << endl;
        cout << "��i�����: ";
        cin.ignore();
        cin.getline(s.lname, 50);

        cout << "����: ";
        cin >> s.kurs;

        int sp;
        cout << "����i����i��� (1 - ����'�����i �����, 2 - I����������, 3 - ���������� �� ������i��, "
            << "4 - �i���� �� i����������, 5 - ������� ��������): ";
        cin >> sp;
        s.spec = static_cast<Spec>(sp);

        cout << "��i��� � �i����: ";
        cin >> s.physics;

        cout << "��i��� � ����������: ";
        cin >> s.math;

        // �������� ������ ������ ������� �� ������������
        switch (s.spec) {
        case CS:
            cout << "��i��� � �������������: ";
            break;
        case IT:
            cout << "��i��� � ��������� ������: ";
            break;
        default:
            cout << "��i��� � ���������: ";
        }
        cin >> s.thirdMark;
        cout << endl;

        file.write(reinterpret_cast<char*>(&s), sizeof(Student));
    }
    file.close();
}

// ��������� ����� � �������� �����
void Print(const char* filename, int amSt) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "������� �������� ����� ��� �������" << endl;
        return;
    }

    cout << "--------------------------------------------------------------------------------------------------------\n";
    cout << "| � | ��i����� | ���� |    ����i����i���    | �i���� | ���������� | �������. | ���. ������ | ��������� |\n";
    cout << "--------------------------------------------------------------------------------------------------------\n";

    Student s;
    for (int i = 0; i < amSt; i++) {
        file.read(reinterpret_cast<char*>(&s), sizeof(Student));

        cout << "| " << setw(1) << right << i + 1 << " ";
        cout << "| " << setw(9) << left << s.lname
            << "| " << setw(5) << right << s.kurs
            << "| " << setw(20) << left << s.return_spec()
            << "| " << setw(7) << right << s.physics
            << "| " << setw(11) << right << s.math;

        switch (s.spec) {
        case CS:
            cout << "| " << setw(9) << right << s.thirdMark
                << "| " << setw(12) << right << " "
                << "| " << setw(11) << right << " ";
            break;
        case IT:
            cout << "| " << setw(9) << right << " "
                << "| " << setw(12) << right << s.thirdMark
                << "| " << setw(11) << right << " ";
            break;
        default:
            cout << "| " << setw(9) << right << " "
                << "| " << setw(12) << right << " "
                << "| " << setw(11) << right << s.thirdMark;
        }
        cout << "|\n";
    }
    cout << "--------------------------------------------------------------------------------------------------------\n";
    file.close();
}

// ���������� ����� � �������� ����
void Sort(const char* filename, int amSt) {
    fstream file(filename, ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "������� �������� �����" << endl;
        return;
    }

    for (int i0 = 0; i0 < amSt - 1; i0++) {
        for (int i1 = 0; i1 < amSt - i0 - 1; i1++) {
            Student s1, s2;

            file.seekg(i1 * sizeof(Student));
            file.read(reinterpret_cast<char*>(&s1), sizeof(Student));

            file.seekg((i1 + 1) * sizeof(Student));
            file.read(reinterpret_cast<char*>(&s2), sizeof(Student));

            bool needSwap =
                (s1.spec > s2.spec) ||
                (s1.spec == s2.spec && s1.math > s2.math) ||
                (s1.spec == s2.spec && s1.math == s2.math && string(s1.lname) > string(s2.lname));

            if (needSwap) {
                file.seekp(i1 * sizeof(Student));
                file.write(reinterpret_cast<char*>(&s2), sizeof(Student));

                file.seekp((i1 + 1) * sizeof(Student));
                file.write(reinterpret_cast<char*>(&s1), sizeof(Student));
            }
        }
    }

    file.close();
    cout << "��� ������������!" << endl;
}

// �������� ����� � ����
int binSearch(const char* filename, int amSt, const string& prizv, Spec spec, int math) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "������� �������� �����" << endl;
        return -1;
    }

    int L = 0, R = amSt - 1;
    while (L <= R) {
        int m = (L + R) / 2;
        Student s;

        file.seekg(m * sizeof(Student));
        file.read(reinterpret_cast<char*>(&s), sizeof(Student));

        if (s.lname == prizv && s.spec == spec && s.math == math)
            return m;

        if ((s.spec < spec) ||
            (s.spec == spec && s.math < math) ||
            (s.spec == spec && s.math == math && string(s.lname) < prizv))
            L = m + 1;
        else
            R = m - 1;
    }

    file.close();
    return -1;
}

int main() {
    SetConsoleCP(1251);
    SetConsoleOutputCP(1251);

    int amSt;
    char filename[100];

    cout << "ʳ������ ��������: ";
    cin >> amSt;

    cout << "������ ����� �����: ";
    cin >> filename;

    int menuItem;
    do {
        cout << endl << endl;
        cout << "������� ��:" << endl << endl;
        cout << " [1] - �������� ����� � ���������" << endl;
        cout << " [2] - ���� ����� �� �����" << endl;
        cout << " [3] - ������� ������������� �����" << endl;
        cout << " [4] - ������� ����� ��������" << endl;
        cout << " [0] - ����� �� ���������� ��������" << endl << endl;
        cout << "������ ��������: ";
        cin >> menuItem;
        cout << endl;

        switch (menuItem) {
        case 1:
            Create(filename, amSt);
            break;
        case 2:
            Print(filename, amSt);
            break;
        case 3:
            Sort(filename, amSt);
            break;
        case 4: {
            string prizv;
            int speciality, mathGrade;
            cout << "������ �������: ";
            cin >> prizv;
            cout << "������ ������������ (1-5): ";
            cin >> speciality;
            cout << "������ ������ � ����������: ";
            cin >> mathGrade;

            int found = binSearch(filename, amSt, prizv, static_cast<Spec>(speciality), mathGrade);
            if (found >= 0)
                cout << "�������� �������� � ������� " << found + 1 << endl;
            else
                cout << "�������� �������� �� ��������" << endl;
            break;
        }
        case 0:
            break;
        default:
            cout << "�������: �������� ����� ����" << endl;
        }
    } while (menuItem != 0);

    return 0;
}