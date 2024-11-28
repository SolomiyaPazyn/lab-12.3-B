#include <iostream>
#include <fstream>
#include <string>
#include <Windows.h>
#include <iomanip>
using namespace std;

// Перелік спеціальностей
enum Spec { CS = 1, IT, ME, PhIT, TR };

// Структура для зберігання інформації про студентів
#pragma pack(push, 1)
struct Student {
    char lname[50];
    int kurs;
    Spec spec;
    int physics;
    int math;
    int thirdMark; // Замість об'єднання використовуємо просто int

    // Метод для повернення назви спеціальності
    string return_spec() {
        switch (spec) {
        case CS: return "Комп'ютернi науки";
        case IT: return "Iнформатика";
        case ME: return "Математика та економiка";
        case PhIT: return "Фiзика та iнформатика";
        case TR: return "Трудове навчання";
        default: return "Невідомо";
        }
    }
};
#pragma pack(pop)

// Створення та збереження даних у бінарний файл
void Create(const char* filename, int amSt) {
    ofstream file(filename, ios::binary);
    if (!file) {
        cerr << "Помилка відкриття файлу для запису" << endl;
        return;
    }

    for (int i = 0; i < amSt; i++) {
        Student s;
        cout << "Студент №" << i + 1 << endl;
        cout << "Прiзвище: ";
        cin.ignore();
        cin.getline(s.lname, 50);

        cout << "Курс: ";
        cin >> s.kurs;

        int sp;
        cout << "Спецiальнiсть (1 - Комп'ютернi науки, 2 - Iнформатика, 3 - Математика та економiка, "
            << "4 - Фiзика та iнформатика, 5 - Трудове навчання): ";
        cin >> sp;
        s.spec = static_cast<Spec>(sp);

        cout << "Оцiнка з фiзики: ";
        cin >> s.physics;

        cout << "Оцiнка з математики: ";
        cin >> s.math;

        // Введення третьої оцінки залежно від спеціальності
        switch (s.spec) {
        case CS:
            cout << "Оцiнка з програмування: ";
            break;
        case IT:
            cout << "Оцiнка з чисельних методів: ";
            break;
        default:
            cout << "Оцiнка з педагогіки: ";
        }
        cin >> s.thirdMark;
        cout << endl;

        file.write(reinterpret_cast<char*>(&s), sizeof(Student));
    }
    file.close();
}

// Виведення даних з бінарного файлу
void Print(const char* filename, int amSt) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Помилка відкриття файлу для читання" << endl;
        return;
    }

    cout << "--------------------------------------------------------------------------------------------------------\n";
    cout << "| № | Прiзвище | Курс |    Спецiальнiсть    | Фiзика | Математика | Програм. | Чис. методи | Педагогіка |\n";
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

// Сортування даних у бінарному файлі
void Sort(const char* filename, int amSt) {
    fstream file(filename, ios::in | ios::out | ios::binary);
    if (!file) {
        cerr << "Помилка відкриття файлу" << endl;
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
    cout << "Дані впорядковано!" << endl;
}

// Бінарний пошук у файлі
int binSearch(const char* filename, int amSt, const string& prizv, Spec spec, int math) {
    ifstream file(filename, ios::binary);
    if (!file) {
        cerr << "Помилка відкриття файлу" << endl;
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

    cout << "Кількість студентів: ";
    cin >> amSt;

    cout << "Введіть назву файлу: ";
    cin >> filename;

    int menuItem;
    do {
        cout << endl << endl;
        cout << "Виберіть дію:" << endl << endl;
        cout << " [1] - введення даних з клавіатури" << endl;
        cout << " [2] - вивід даних на екран" << endl;
        cout << " [3] - фізичне впорядкування даних" << endl;
        cout << " [4] - бінарний пошук студента" << endl;
        cout << " [0] - вихід та завершення програми" << endl << endl;
        cout << "Введіть значення: ";
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
            cout << "Введіть прізвище: ";
            cin >> prizv;
            cout << "Введіть спеціальність (1-5): ";
            cin >> speciality;
            cout << "Введіть оцінку з математики: ";
            cin >> mathGrade;

            int found = binSearch(filename, amSt, prizv, static_cast<Spec>(speciality), mathGrade);
            if (found >= 0)
                cout << "Знайдено студента в позиції " << found + 1 << endl;
            else
                cout << "Шуканого студента не знайдено" << endl;
            break;
        }
        case 0:
            break;
        default:
            cout << "Помилка: Невідомий пункт меню" << endl;
        }
    } while (menuItem != 0);

    return 0;
}