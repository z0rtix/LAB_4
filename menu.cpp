#include "menu.h"

#include <iostream>
#include <string>
#include <chrono>
#include <stdexcept>

#include "Cardinal.h"
#include "LazySequence.h"
#include "Stream.h"
#include "StateMachine.h"
#include "tests/tests.h"

using namespace std;

// ---------- вспомогательный ввод ----------
static void clearInput() {
	cin.clear();
	cin.ignore(100000, '\n');
}

static int readInt(const string& msg) {
	int value;
	while (true) {
		cout << msg;
		if (cin >> value) {
			return value;
		}
		cout << "Некорректный ввод\n";
		clearInput();
	}
}

static long long readLongLong(const string& msg) {
	long long value;
	while (true) {
		cout << msg;
		if (cin >> value) {
			return value;
		}
		cout << "Некорректный ввод\n";
		clearInput();
	}
}

static void pauseConsole() {
	cout << "\nНажмите ENTER для продолжения...";
	clearInput();
	cin.get();
}

// ---------- печать последовательностей ----------
template <class T>
static void printFiniteSequence(const LazySequence<T>& seq, int limit = 30) {
	cout << "[ ";
	int len = seq.getLength();
	for (int i = 0; i < len && i < limit; i++) {
		cout << seq.get(i) << " ";
	}
	if (len > limit) {
		cout << "... ";
	}
	cout << "]";
}

template <class T>
static void printInfiniteSequence(const LazySequence<T>& seq, int limit = 20) {
	cout << "[ ";
	for (int i = 0; i < limit; i++) {
		cout << seq.get(i) << " ";
	}
	cout << "... ]";
}

template <class T>
static void printSequenceInfo(const LazySequence<T>& seq) {
	cout << "\nТекущая последовательность:\n";
	if (seq.cardinalLength().IsFinite()) {
		printFiniteSequence(seq);
		cout << "\nДлина: " << seq.getLength() << "\n";
	} else {
		printInfiniteSequence(seq);
		cout << "\nДлина: бесконечная\n";
	}
	cout << "Материализовано элементов: " << seq.getMaterializedCount() << "\n";
}

// ---------- готовые генераторы ----------
static LazySequence<long long>* createNaturalSequence() {
	MutableArraySequence<long long> init;
	init.append(0);
	auto gen = [](const Sequence<long long>& s) -> long long {
		return s.getLast() + 1;
	};
	return new LazySequence<long long>(gen, init, Cardinal::Infinite());
}

static LazySequence<long long>* createPowerSequence() {
	MutableArraySequence<long long> init;
	init.append(1);
	auto gen = [](const Sequence<long long>& s) -> long long {
		return s.getLast() * 2;
	};
	return new LazySequence<long long>(gen, init, Cardinal::Infinite());
}

static LazySequence<long long>* createFactorialSequence() {
	MutableArraySequence<long long> init;
	init.append(1);
	auto gen = [](const Sequence<long long>& s) -> long long {
		return s.getLast() * (s.getLength() + 1);
	};
	return new LazySequence<long long>(gen, init, Cardinal::Infinite());
}

static LazySequence<long long>* createFibonacciSequence() {
	MutableArraySequence<long long> init;
	init.append(0);
	init.append(1);
	auto gen = [](const Sequence<long long>& s) -> long long {
		int n = s.getLength();
		return s.get(n - 1) + s.get(n - 2);
	};
	return new LazySequence<long long>(gen, init, Cardinal::Infinite());
}

// ---------- ручное создание последовательности ----------
static LazySequence<long long>* manualCreateSequence() {
	cout << "\n====================================================\n";
	cout << " СОЗДАНИЕ ПОСЛЕДОВАТЕЛЬНОСТИ\n";
	cout << "====================================================\n\n";
	cout << "1. Натуральные числа\n";
	cout << "2. Степени двойки\n";
	cout << "3. Факториалы\n";
	cout << "4. Числа Фибоначчи\n";
	cout << "5. Конечная последовательность\n";
	cout << "0. Отмена\n";

	int choice = readInt("\nВыберите тип последовательности: ");
	if (choice == 0) {
		return nullptr;
	}
	if (choice == 1) {
		cout << "\nСоздана бесконечная последовательность натуральных чисел\n";
		return createNaturalSequence();
	}
	if (choice == 2) {
		cout << "\nСоздана бесконечная последовательность степеней двойки\n";
		return createPowerSequence();
	}
	if (choice == 3) {
		cout << "\nСоздана бесконечная последовательность факториалов\n";
		return createFactorialSequence();
	}
	if (choice == 4) {
		cout << "\nСоздана бесконечная последовательность Фибоначчи\n";
		return createFibonacciSequence();
	}
	if (choice == 5) {
		int count = readInt("Введите количество элементов: ");
		while (count <= 0) {
			cout << "Количество должно быть положительным\n";
			count = readInt("Введите количество элементов: ");
		}
		MutableArraySequence<long long> arr;
		for (int i = 0; i < count; i++) {
			long long value = readLongLong("Введите элемент: ");
			arr.append(value);
		}
		cout << "\nСоздана конечная последовательность\n";
		return new LazySequence<long long>(arr);
	}
	cout << "\nНеизвестный пункт меню\n";
	return nullptr;
}

// ---------- демонстрация Фибоначчи ----------
static void demoFibonacci() {
	cout << "\n====================================================\n";
	cout << " ДЕМОНСТРАЦИЯ БЕСКОНЕЧНОЙ ПОСЛЕДОВАТЕЛЬНОСТИ ФИБОНАЧЧИ\n";
	cout << "====================================================\n";

	LazySequence<long long>* fib = createFibonacciSequence();
	cout << "\nПервые 20 чисел Фибоначчи:\n";
	printInfiniteSequence(*fib, 20);
	cout << "\n\nДобавляем элементы 777, 888, 999 в конец последовательности\n";

	LazySequence<long long>* tmp1 = fib->append(777);
	LazySequence<long long>* tmp2 = tmp1->append(888);
	LazySequence<long long>* tmp3 = tmp2->append(999);
	delete tmp1;
	delete tmp2;

	cout << "\nОрдинальные индексы:\n";
	cout << "ω     = " << tmp3->get(Cardinal::InfinitePlus(0)) << "\n";
	cout << "ω + 1 = " << tmp3->get(Cardinal::InfinitePlus(1)) << "\n";
	cout << "ω + 2 = " << tmp3->get(Cardinal::InfinitePlus(2)) << "\n";

	try {
		cout << "\nПопытка обращения к ω + 5:\n";
		cout << tmp3->get(Cardinal::InfinitePlus(5)) << "\n";
	} catch (const exception& e) {
		cout << "Ожидаемое исключение: " << e.what() << "\n";
	}

	cout << "\nПрименяем map (умножение каждого элемента на 2)\n";
	LazySequence<long long>* mapped = tmp3->map([](long long x) { return x * 2; });
	cout << "\nПервые 15 значений после map:\n";
	printInfiniteSequence(*mapped, 15);
	cout << "\n\nКоличество материализованных элементов:\n";
	cout << "Исходная последовательность: " << fib->getMaterializedCount() << "\n";
	cout << "После map: " << mapped->getMaterializedCount() << "\n";

	delete fib;
	delete tmp3;
	delete mapped;
	pauseConsole();
}

// ---------- демонстрация автомата ----------
static void demoStateMachine() {
	cout << "\n====================================================\n";
	cout << " ДЕМОНСТРАЦИЯ АВТОМАТА СОСТОЯНИЙ\n";
	cout << "====================================================\n";
	cout << "\nАвтомат имеет два состояния: 0 и 1\n";
	cout << "Таблица переходов:\n\n";
	cout << "Состояние 0 + символ 0 -> состояние 0, выход 0\n";
	cout << "Состояние 0 + символ 1 -> состояние 1, выход 1\n";
	cout << "Состояние 1 + символ 0 -> состояние 1, выход 1\n";
	cout << "Состояние 1 + символ 1 -> состояние 0, выход 0\n";

	StateMachine<int, int> sm;
	sm.AddTransition(0, 0, 0, 0);
	sm.AddTransition(0, 1, 1, 1);
	sm.AddTransition(1, 0, 1, 1);
	sm.AddTransition(1, 1, 0, 0);
	sm.SetInitialState(0);

	int arr[] = {1, 1, 0, 1, 0, 0, 1};
	MutableArraySequence<int> inputArr;
	for (int i = 0; i < 7; i++) {
		inputArr.append(arr[i]);
	}
	LazySequence<int> input(inputArr);
	ReadOnlyStream<int> inputStream(&input);
	MutableArraySequence<int> outputArr;
	WriteOnlyStream<int> outputStream(&outputArr);
	sm.ProcessStream(inputStream, outputStream);
	LazySequence<int> output(outputArr);

	cout << "\nВходная последовательность:\n";
	printFiniteSequence(input);
	cout << "\n\nВыходная последовательность:\n";
	printFiniteSequence(output);
	cout << "\n";
	pauseConsole();
}

// ---------- операции ----------
static void applyAppend(LazySequence<long long>*& seq) {
	cout << "\nappend(value)\n";
	cout << "Добавляет элемент в конец последовательности\n\n";
	long long value = readLongLong("Введите значение: ");
	LazySequence<long long>* tmp = seq->append(value);
	delete seq;
	seq = tmp;
	cout << "\nЭлемент успешно добавлен\n";
	printSequenceInfo(*seq);
}

static void applyPrepend(LazySequence<long long>*& seq) {
	cout << "\nprepend(value)\n";
	cout << "Добавляет элемент в начало последовательности\n\n";
	long long value = readLongLong("Введите значение: ");
	LazySequence<long long>* tmp = seq->prepend(value);
	delete seq;
	seq = tmp;
	cout << "\nЭлемент успешно добавлен в начало\n";
	printSequenceInfo(*seq);
}

static void applyInsertAt(LazySequence<long long>*& seq) {
	cout << "\ninsertAt(value, index)\n";
	cout << "Вставляет элемент по указанному индексу\n\n";
	long long value = readLongLong("Введите значение: ");
	int index = readInt("Введите индекс: ");
	LazySequence<long long>* tmp = seq->insertAt(value, index);
	delete seq;
	seq = tmp;
	cout << "\nЭлемент успешно вставлен\n";
	printSequenceInfo(*seq);
}

static void applySet(LazySequence<long long>*& seq) {
	cout << "\nset(value, index)\n";
	cout << "Изменяет элемент по указанному индексу\n\n";
	long long value = readLongLong("Введите новое значение: ");
	int index = readInt("Введите индекс: ");
	LazySequence<long long>* tmp = seq->set(value, index);
	delete seq;
	seq = tmp;
	cout << "\nЭлемент успешно изменён\n";
	printSequenceInfo(*seq);
}

static void applyRemoveAt(LazySequence<long long>*& seq) {
	cout << "\nremoveAt(index)\n";
	cout << "Удаляет элемент по индексу\n\n";
	int index = readInt("Введите индекс: ");
	LazySequence<long long>* tmp = seq->removeAt(index);
	delete seq;
	seq = tmp;
	cout << "\nЭлемент успешно удалён\n";
	printSequenceInfo(*seq);
}

static void applySubsequence(LazySequence<long long>* seq) {
	cout << "\ngetSubsequence(start, end)\n";
	cout << "Возвращает подпоследовательность\n\n";
	int start = readInt("Введите начало: ");
	int end = readInt("Введите конец: ");
	LazySequence<long long>* sub = seq->getSubsequence(start, end);
	cout << "\nПолученная подпоследовательность:\n";
	printFiniteSequence(*sub);
	cout << "\n";
	delete sub;
}

static void applyMap(LazySequence<long long>* seq) {
	cout << "\nmap(function)\n";
	cout << "Применяет функцию к каждому элементу (x * 2)\n\n";
	LazySequence<long long>* mapped = seq->map([](long long x) { return x * 2; });
	cout << "Результат:\n";
	if (mapped->cardinalLength().IsFinite()) {
		printFiniteSequence(*mapped);
	} else {
		printInfiniteSequence(*mapped);
	}
	cout << "\n";
	delete mapped;
}

static void applyWhere(LazySequence<long long>* seq) {
	cout << "\nwhere(predicate)\n";
	cout << "Оставляет только чётные элементы\n\n";
	LazySequence<long long>* filtered = seq->where([](long long x) { return x % 2 == 0; });
	cout << "Результат фильтрации:\n";
	printFiniteSequence(*filtered);
	cout << "\n";
	delete filtered;
}

static void applyConcat(LazySequence<long long>*& seq) {
	cout << "\nconcat(sequence)\n";
	cout << "Объединяет текущую последовательность с натуральными числами\n\n";
	LazySequence<long long>* natural = createNaturalSequence();
	LazySequence<long long>* result = seq->concat(natural);
	delete natural;
	delete seq;
	seq = result;
	cout << "\nПоследовательности успешно объединены\n";
	if (seq->cardinalLength().IsFinite()) {
		printFiniteSequence(*seq);
	} else {
		printInfiniteSequence(*seq);
	}
	cout << "\n";
}

// ---------- песочница ----------
static void playground() {
	LazySequence<long long>* seq = manualCreateSequence();
	if (!seq) {
		return;
	}
	while (true) {
		cout << "\n====================================================\n";
		cout << " ПЕСОЧНИЦА ОПЕРАЦИЙ\n";
		cout << "====================================================\n";
		printSequenceInfo(*seq);
		cout << "\n1. append\n";
		cout << "2. prepend\n";
		cout << "3. insertAt\n";
		cout << "4. set\n";
		cout << "5. removeAt\n";
		cout << "6. getSubsequence\n";
		cout << "7. map\n";
		cout << "8. where\n";
		cout << "9. concat\n";
		cout << "10. Создать новую последовательность\n";
		cout << "0. Вернуться в главное меню\n";

		int cmd = readInt("\nВыберите операцию: ");
		if (cmd == 0) {
			break;
		}
		try {
			if (cmd == 1) {
				applyAppend(seq);
			} else if (cmd == 2) {
				applyPrepend(seq);
			} else if (cmd == 3) {
				applyInsertAt(seq);
			} else if (cmd == 4) {
				applySet(seq);
			} else if (cmd == 5) {
				applyRemoveAt(seq);
			} else if (cmd == 6) {
				applySubsequence(seq);
			} else if (cmd == 7) {
				applyMap(seq);
			} else if (cmd == 8) {
				applyWhere(seq);
			} else if (cmd == 9) {
				applyConcat(seq);
			} else if (cmd == 10) {
				delete seq;
				seq = manualCreateSequence();
				if (!seq) {
					break;
				}
			} else {
				cout << "\nНеизвестная команда\n";
			}
		} catch (const exception& e) {
			cout << "\nОшибка: " << e.what() << "\n";
		}
	}
	delete seq;
}

// ---------- глобальная последовательность ----------
static LazySequence<long long>* globalSequence = nullptr;

static void manualOperations() {
	if (!globalSequence) {
		cout << "\nТекущая последовательность отсутствует\n";
		globalSequence = manualCreateSequence();
		if (!globalSequence) {
			return;
		}
	}
	while (true) {
		cout << "\n====================================================\n";
		cout << " РУЧНЫЕ ОПЕРАЦИИ НАД ПОСЛЕДОВАТЕЛЬНОСТЬЮ\n";
		cout << "====================================================\n";
		printSequenceInfo(*globalSequence);
		cout << "\n1. append\n";
		cout << "2. prepend\n";
		cout << "3. insertAt\n";
		cout << "4. set\n";
		cout << "5. removeAt\n";
		cout << "6. getSubsequence\n";
		cout << "7. map\n";
		cout << "8. where\n";
		cout << "9. concat\n";
		cout << "10. Создать новую последовательность\n";
		cout << "0. Назад\n";

		int cmd = readInt("\nВыберите операцию: ");
		if (cmd == 0) {
			return;
		}
		try {
			if (cmd == 1) {
				applyAppend(globalSequence);
			} else if (cmd == 2) {
				applyPrepend(globalSequence);
			} else if (cmd == 3) {
				applyInsertAt(globalSequence);
			} else if (cmd == 4) {
				applySet(globalSequence);
			} else if (cmd == 5) {
				applyRemoveAt(globalSequence);
			} else if (cmd == 6) {
				applySubsequence(globalSequence);
			} else if (cmd == 7) {
				applyMap(globalSequence);
			} else if (cmd == 8) {
				applyWhere(globalSequence);
			} else if (cmd == 9) {
				applyConcat(globalSequence);
			} else if (cmd == 10) {
				delete globalSequence;
				globalSequence = manualCreateSequence();
			}
		} catch (const exception& e) {
			cout << "\nОшибка: " << e.what() << "\n";
		}
	}
}

// ---------- запуск тестов ----------
static void runAllTestsMenu() {
	cout << "\n====================================================\n";
	cout << " ЗАПУСК ВСЕХ ТЕСТОВ\n";
	cout << "====================================================\n\n";
	runAllTests();
	cout << "\nВсе тесты успешно завершены\n";
	pauseConsole();
}

// ---------- стресс-тест ----------
static void runFibonacciStress() {
	cout << "\n====================================================\n";
	cout << " СТРЕСС-ТЕСТ ФИБОНАЧЧИ\n";
	cout << "====================================================\n\n";
	cout << "Генерация 1 000 000 чисел Фибоначчи\n";
	LazySequence<long long>* fib = createFibonacciSequence();
	auto start = chrono::high_resolution_clock::now();
	volatile long long x = 0;
	for (int i = 0; i < 1000000; i++) {
		x = fib->get(i);
	}
	auto end = chrono::high_resolution_clock::now();
	auto ms = chrono::duration_cast<chrono::milliseconds>(end - start).count();
	cout << "\nВремя выполнения: " << ms << " мс\n";
	cout << "Материализовано элементов: " << fib->getMaterializedCount() << "\n";
	delete fib;
	pauseConsole();
}

// ---------- таблица производительности ----------
template <class Func>
static long long measure(Func f) {
	auto start = chrono::high_resolution_clock::now();
	f();
	auto end = chrono::high_resolution_clock::now();
	return chrono::duration_cast<chrono::milliseconds>(end - start).count();
}

static void runPerformanceTable() {
	cout << "\n====================================================\n";
	cout << " ТАБЛИЦА ПРОИЗВОДИТЕЛЬНОСТИ\n";
	cout << "====================================================\n\n";

	long long lazyInt = measure([]() { TestLazySequence<int>("int"); });
	long long lazyDouble = measure([]() { TestLazySequence<double>("double"); });
	long long streamInt = measure([]() { TestStream<int>("int"); });
	long long streamDouble = measure([]() { TestStream<double>("double"); });
	long long fsmInt = measure([]() { TestStateMachine<int, int>("int-int"); });
	long long fsmDouble = measure([]() { TestStateMachine<int, double>("int-double"); });

	cout << "------------------------------------------------------------\n";
	cout << " Компонент                         Время (мс)\n";
	cout << "------------------------------------------------------------\n";
	cout << " LazySequence<int>                 " << lazyInt << "\n";
	cout << " LazySequence<double>              " << lazyDouble << "\n";
	cout << " Stream<int>                       " << streamInt << "\n";
	cout << " Stream<double>                    " << streamDouble << "\n";
	cout << " StateMachine<int,int>             " << fsmInt << "\n";
	cout << " StateMachine<int,double>          " << fsmDouble << "\n";
	cout << "------------------------------------------------------------\n";
	pauseConsole();
}

// ---------- главное меню ----------
void runMenu() {
	while (true) {
		cout << "\n====================================================\n";
		cout << " ЛАБОРАТОРНАЯ РАБОТА\n";
		cout << " ЛЕНИВЫЕ ПОСЛЕДОВАТЕЛЬНОСТИ И АВТОМАТЫ\n";
		cout << "====================================================\n\n";
		cout << "1. Демонстрация бесконечной последовательности Фибоначчи\n";
		cout << "   (Fibonacci demo)\n\n";
		cout << "2. Демонстрация автомата состояний\n";
		cout << "   (State machine demo)\n\n";
		cout << "3. Создать новую последовательность\n";
		cout << "   (New sequence)\n\n";
		cout << "4. Песочница операций\n";
		cout << "   (Playground)\n\n";
		cout << "5. Ручные операции над выбранной последовательностью\n";
		cout << "   (Manual operations)\n\n";
		cout << "6. Запустить все тесты\n";
		cout << "   (Run all tests)\n\n";
		cout << "7. Стресс-тест Фибоначчи\n";
		cout << "   (Fibonacci stress test)\n\n";
		cout << "8. Таблица производительности\n";
		cout << "   (Performance table)\n\n";
		cout << "0. Выход\n";
		cout << "   (Exit)\n";

		int cmd = readInt("\nВыберите пункт меню: ");
		if (cmd == 0) {
			break;
		}
		if (cmd == 1) {
			demoFibonacci();
		} else if (cmd == 2) {
			demoStateMachine();
		} else if (cmd == 3) {
			LazySequence<long long>* seq = manualCreateSequence();
			if (seq) {
				cout << "\nСозданная последовательность:\n";
				printSequenceInfo(*seq);
				delete seq;
				pauseConsole();
			}
		} else if (cmd == 4) {
			playground();
		} else if (cmd == 5) {
			manualOperations();
		} else if (cmd == 6) {
			runAllTestsMenu();
		} else if (cmd == 7) {
			runFibonacciStress();
		} else if (cmd == 8) {
			runPerformanceTable();
		} else {
			cout << "\nНеизвестный пункт меню\n";
		}
	}
	delete globalSequence;
}