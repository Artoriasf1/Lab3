#include <cmath>
#include <initializer_list>
#include <iostream>
#include <memory>
#include <sstream>
#include <stdexcept>
#include <string>
#include <utility>
#include <vector>

#ifdef _WIN32
#include <windows.h>
#endif

class Name {
 public:
  explicit Name(std::string first_name)
      : first_name_(std::move(first_name)) {}

  Name(std::string surname, std::string first_name,
       std::string patronymic = "")
      : surname_(std::move(surname)),
        first_name_(std::move(first_name)),
        patronymic_(std::move(patronymic)) {}

  std::string ToString() const {
    std::string result;
    const std::vector<std::string> parts = {
        surname_, first_name_, patronymic_};

    for (const std::string& part : parts) {
      if (part.empty()) {
        continue;
      }
      if (!result.empty()) {
        result += " ";
      }
      result += part;
    }

    return result;
  }

  Name WithFather(const Name& father_name) const {
    Name result = *this;

    if (result.surname_.empty()) {
      result.surname_ = father_name.surname_;
    }

    if (result.patronymic_.empty() && !father_name.first_name_.empty()) {
      result.patronymic_ = father_name.first_name_ + "ович";
    }

    return result;
  }

  void Print() const {
    std::cout << ToString() << '\n';
  }

 private:
  std::string surname_;
  std::string first_name_;
  std::string patronymic_;
};

class Person {
 public:
  Person(Name name, int height)
      : name_(std::move(name)),
        height_(height),
        has_height_(true),
        father_(nullptr) {}

  explicit Person(Name name)
      : name_(std::move(name)),
        height_(0),
        has_height_(false),
        father_(nullptr) {}

  Person(Name name, const Person& father)
      : name_(std::move(name)),
        height_(0),
        has_height_(false),
        father_(&father) {}

  std::string ToString() const {
    const std::string full_name = GetEffectiveName().ToString();

    if (!has_height_) {
      return full_name;
    }

    return full_name + ", рост: " + std::to_string(height_);
  }

  void Print() const {
    std::cout << ToString() << '\n';
  }

 private:
  Name GetEffectiveName() const {
    if (father_ == nullptr) {
      return name_;
    }

    return name_.WithFather(father_->GetEffectiveName());
  }

  Name name_;
  int height_;
  bool has_height_;
  const Person* father_;
};

class Point {
 public:
  Point(double x, double y) : x_(x), y_(y) {}

  std::string ToString() const {
    std::ostringstream output;
    output << "{" << x_ << ";" << y_ << "}";
    return output.str();
  }

  double DistanceTo(const Point& other) const {
    return std::hypot(x_ - other.x_, y_ - other.y_);
  }

  void MoveBy(double delta_x, double delta_y) {
    x_ += delta_x;
    y_ += delta_y;
  }

  void Print() const {
    std::cout << ToString() << '\n';
  }

 private:
  double x_;
  double y_;
};

class PolylineShape {
 public:
  PolylineShape() = default;

  explicit PolylineShape(const std::vector<Point>& points) {
    for (const Point& point : points) {
      points_.push_back(std::make_shared<Point>(point));
    }
  }

  static PolylineShape CreateSharedPoints(
      const std::vector<std::shared_ptr<Point>>& points) {
    PolylineShape polyline;
    polyline.points_ = points;
    return polyline;
  }

  std::string ToString() const {
    std::ostringstream output;
    output << "Линия [";

    for (std::size_t index = 0; index < points_.size(); index += 1) {
      if (index > 0) {
        output << ", ";
      }
      output << points_[index]->ToString();
    }

    output << "]";
    return output.str();
  }

  void AddPoints(std::initializer_list<Point> new_points) {
    for (const Point& point : new_points) {
      points_.push_back(std::make_shared<Point>(point));
    }
  }

  void AddPoints(const std::vector<Point>& new_points) {
    for (const Point& point : new_points) {
      points_.push_back(std::make_shared<Point>(point));
    }
  }

  void MovePoint(std::size_t index, double delta_x, double delta_y) {
    if (index >= points_.size()) {
      throw std::out_of_range(
          "Индекс точки "
          "находится вне диапазона.");
    }

    points_[index]->MoveBy(delta_x, delta_y);
  }

  std::shared_ptr<Point> GetPointPointer(std::size_t index) const {
    if (index >= points_.size()) {
      throw std::out_of_range(
          "Индекс точки "
          "находится вне диапазона.");
    }

    return points_[index];
  }

  double Length() const {
    double total_length = 0.0;

    for (std::size_t index = 1; index < points_.size(); index += 1) {
      total_length += points_[index - 1]->DistanceTo(*points_[index]);
    }

    return total_length;
  }

  void Print() const {
    std::cout << ToString() << '\n';
  }

 private:
  std::vector<std::shared_ptr<Point>> points_;
};

int main() {
#ifdef _WIN32
  SetConsoleOutputCP(CP_UTF8);
  SetConsoleCP(CP_UTF8);
#endif

  std::cout << "Лабораторная работа №3. "
            << "Вариант 10\n";
  std::cout << "====================================\n\n";

  // Задание 1.3. Имена.
  std::cout << "Задание 1.3. Имена\n";
  const Name cleopatra("Клеопатра");
  const Name pushkin("Пушкин", "Александр",
                     "Сергеевич");
  const Name mayakovsky("Маяковский", "Владимир");

  cleopatra.Print();
  pushkin.Print();
  mayakovsky.Print();

  // Задание 2.2. Человек с именем.
  std::cout << "\nЗадание 2.2. Человек с именем\n";
  const Person person_cleopatra(cleopatra, 152);
  const Person person_pushkin(pushkin, 167);
  const Person person_mayakovsky(mayakovsky, 189);

  person_cleopatra.Print();
  person_pushkin.Print();
  person_mayakovsky.Print();

  // Задание 2.3. Человек с родителем.
  std::cout << "\nЗадание 2.3. Человек с родителем\n";
  const Person ivan(Name("Чудов", "Иван"));
  const Person petr(Name("Чудов", "Пётр"), ivan);
  const Person boris(Name("Борис"), petr);

  std::cout << "Иван: ";
  ivan.Print();
  std::cout << "Пётр: ";
  petr.Print();
  std::cout << "Борис: ";
  boris.Print();

  // Задание 3.2. Общие крайние точки ломаных.
  std::cout << "\nЗадание 3.2. Ломаная\n";
  PolylineShape first_polyline({Point(1, 5), Point(2, 8), Point(5, 3)});

  const std::vector<std::shared_ptr<Point>> second_points = {
      first_polyline.GetPointPointer(0),
      std::make_shared<Point>(2, -5),
      std::make_shared<Point>(4, -8),
      first_polyline.GetPointPointer(2)};
  PolylineShape second_polyline = PolylineShape::CreateSharedPoints(second_points);

  std::cout << "Первая ломаная до сдвига: ";
  first_polyline.Print();
  std::cout << "Вторая ломаная до сдвига: ";
  second_polyline.Print();

  first_polyline.MovePoint(0, 4, 2);

  std::cout << "Первая ломаная после сдвига "
            << "начала: ";
  first_polyline.Print();
  std::cout << "Вторая ломаная после сдвига "
            << "первой: ";
  second_polyline.Print();

  // Задание 4.9. Разные конструкторы ломаной.
  std::cout << "\nЗадание 4.9. Создаём ломаную\n";
  const PolylineShape empty_polyline;
  const PolylineShape filled_polyline(
      std::vector<Point>{Point(1, 5), Point(2, 8), Point(5, 3)});

  std::cout << "Пустая ломаная: ";
  empty_polyline.Print();
  std::cout << "Ломаная с точками: ";
  filled_polyline.Print();

  // Задание 5.7. Длина и расширение ломаной.
  std::cout << "\nЗадание 5.7. Длина ломаной\n";
  PolylineShape measured_polyline(
      std::vector<Point>{Point(1, 5), Point(2, 8), Point(5, 3)});

  std::cout << "Исходная ломаная: ";
  measured_polyline.Print();
  std::cout << "Начальная длина: "
            << measured_polyline.Length() << '\n';

  measured_polyline.AddPoints({Point(5, 15), Point(8, 10)});

  std::cout << "Ломаная после добавления точек: ";
  measured_polyline.Print();
  std::cout << "Новая длина: " << measured_polyline.Length() << '\n';

  return 0;
}
