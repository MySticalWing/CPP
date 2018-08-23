/**
 * brief : 定义了InternalSort类。并以静态方法的形式给出了常用内部排序的实现
 * Created by mysticalwing on 18-4-4.
 */

#ifndef INTERNAL_SORT_H
#define INTERNAL_SORT_H

#include <iostream>
#include <vector>

template<typename T>
class InternalSort {
public:
	typedef bool (*compare_function)(const T &a, const T &b);

	/*
	 * *****************************************插入排序********************************************
	 * 1. 直接插入排序
	 * 2. 折半插入排序
	 * 3. 希尔排序
	 * *********************************************************************************************
	 * /

	/*
	 * brief : 直接插入排序
	 * param raw : 原始无序数据，函数会改变其内部数据排序
	 * parrm fun : 函数指针，指向排序时的比较函数，默认升序排列
	 */
	static void insert(std::vector<T> &raw, compare_function fun = default_compare);

	/*
	 * brief : 折半插入排序
	 * param raw : 原始无序数据，函数会改变其内部数据排序
	 * parrm fun : 函数指针，指向排序时的比较函数，默认升序排列
	 */
	static void binaryInsert(std::vector<T> &raw, compare_function fun = default_compare);

	/*
	 * brief : 希尔排序
	 * param raw : 原始无序数据，函数会改变其内部数据排序
	 * param deltas : 希尔排序增量序列
	 * parrm fun : 函数指针，指向排序时的比较函数，默认升序排列
	 */
	static void shellInsert(std::vector<T> &raw, const std::vector<int> &deltas = std::vector<int>(),
	                        compare_function fun = default_compare);


	/*
	 * ********************************************快速排序******************************************
	 * 1. 冒泡排序
	 * 2. 快速排序
	 * *********************************************************************************************
	 */

	/*
	 * brief : 冒泡排序
	 * param raw : 原始无序数据，函数会改变其内部数据排序
	 * parrm fun : 函数指针，指向排序时的比较函数，默认升序排列
	 */
	static void bubble(std::vector<T> &raw, compare_function fun = default_compare);

	/*
	 * brief : 快速排序对外接口
	 * param raw : 原始无序数据，函数会改变其内部数据排序
	 * parrm fun : 函数指针，指向排序时的比较函数，默认升序排列
	 */
	static void quick(std::vector<T> &raw, compare_function fun = default_compare) {
		_quick(raw, 0, raw.size() - 1, fun);
	}

	/*
	 * ********************************************选择排序******************************************
	 * 1. 简单选择排序
	 * 2. 堆排序
	 * *********************************************************************************************
	 */

	/*
	 * brief : 简单选择排序
	 * param raw : 原始无序数据，函数会改变其内部数据排序
	 * parrm fun : 函数指针，指向排序时的比较函数，默认升序排列
	 */
	static void simpleSelection(std::vector<T> &raw, compare_function fun = default_compare);

	/*
	 * brief : 堆排序
	 * param raw : 原始无序数据，函数会改变其内部数据排序
	 * parrm fun : 函数指针，指向排序时的比较函数，默认升序排列
	 */
	static void heap(std::vector<T> &raw, compare_function fun = default_compare);

	/*
	 * *******************************************归并排序********************************************
	 * 1. 递归实现归并排序
	 * 2. 非递归(迭代)实现归并排序
	 * ***********************************************************************************************
	 */

	/*
	 * brief : 归并排序的递归实现
	 */
	static void mergeSort(std::vector<T> &raw, compare_function fun = default_compare);

	/*
	 * brief : 归并算法的非递归(迭代)实现
	 */
	static void mergeSort2(std::vector<T> &raw, compare_function fun = default_compare);

private :
	/*
	 * brief : 希尔排序
	 */
	static void _shellInsert(std::vector<T> &raw, int dleta, compare_function fun);

	// brief : 默认比较函数
	static bool default_compare(const T &a, const T &b) {
		return a < b;
	}

	/*
	 * brief : 交换a，b的值
	 */
	static void _swap(T &a, T &b);

	/*
	 * brief : 寻找快速排序枢轴位置，并返回其下标
	 * param raw : 原始无序数列
	 * param low : 序列下标下界
	 * param high : 序列下标上界
	 * param fun : 用于比较的函数指针
	 * return int : povitkey的下标
	 */
	static int _pivotIndex(std::vector<T> &raw, int low, int high, compare_function fun);

	/*
	 * brief : 快速排序递归处理函数
	 */
	static void _quick(std::vector<T> &raw, int low, int high, compare_function fun);

	/*
	 * brief : 堆调整函数，heap[low, hight]除heap[low]外均满足堆的定义
	 *         本函数调整heap[low]，使heap[low, high]成为一个大/小顶堆
	 * note : 堆采用顺寻存储结构哦
	 * param heap : 堆
	 * param low : 下界
	 * param high : 上界
	 */
	static void _heapAdjust(std::vector<T> &heap, int low, int high, compare_function fun);

	/*
	 * brief : 对两个有序数组arr1和arr2进行合并操作，使得合并后的数组依旧有序
	 * param raw : 原始数组
	 * param aux_arr : 辅助数组，用于提供辅助空间
	 * param low : 下界
	 * param mid : 分割下标
	 * param high : 上界
	 * param fun : 函数指针，指向用于进行比较的函数
	 */
	static void
	_mergeArray(std::vector<T> &raw, std::vector<T> &aux_arr, const int low, const int mid, const int high, compare_function fun);

	/*
	 * brief : 归并排序的递归实现，此函数会改变参数raw中的数据排列顺序
	 * param raw : 原始未排序数组
	 * parma aux_array : 辅助排序数组
	 * param low : 待排序数组下界下标
	 * param high : 待排序数组上界下标
	 * param fun : 函数指针，指向用于进行比较的函数
	 */
	static void _mergeSort(std::vector<T> &raw, std::vector<T> &aux_array, int low, int high, compare_function fun);
};

template<typename T>
void InternalSort<T>::insert(std::vector<T> &raw, InternalSort::compare_function fun) {
	int i, j;
	//插入哨兵
	raw.insert(raw.begin(), 0);
	for (i = 2; i < raw.size(); i++) {
		if (fun(raw[i], raw[i - 1])) {
			raw[0] = raw[i];    //复制为哨兵
			/*将第i个元素插入已有序的[1...i-1]序列*/
			raw[i] = raw[i - 1];
			//找到插入位置
			for (j = i - 2; fun(raw[0], raw[j]); j--)
				raw[j + 1] = raw[j];  //记录后移
			//插入
			raw[j + 1] = raw[0];
		}
	}
	//撤去哨兵
	raw = std::vector<T>(raw.begin() + 1, raw.end());
}

template<typename T>
void InternalSort<T>::binaryInsert(std::vector<T> &raw, InternalSort::compare_function fun) {
	int i, j, low, high, mid;
	//插入哨兵
	raw.insert(raw.begin(), 0);
	for (i = 2; i < raw.size(); i++) {
		if (!fun(raw[i], raw[i - 1]))
			continue;
		raw[0] = raw[i];
		low = 1, high = i - 1;
		/*找到插入位置*/
		while (low <= high) {
			mid = (high + low) / 2;
			if (fun(raw[0], raw[mid]))
				high = mid - 1;
			else
				low = mid + 1;
		}
		//元素后移
		for (j = i - 1; j >= high + 1; j--)
			raw[j + 1] = raw[j];
		raw[high + 1] = raw[0];   //插入
	}
	//撤去哨兵
	raw = std::vector<T>(raw.begin() + 1, raw.end());
}

template<typename T>
void
InternalSort<T>::shellInsert(std::vector<T> &raw, const std::vector<int> &deltas, InternalSort::compare_function fun) {
	std::vector<int> dt(deltas.begin(), deltas.end());
	if (dt.empty()) {
		dt.push_back(raw.size() / 2);
		dt.push_back(5);
		dt.push_back(3);
		dt.push_back(2);
		dt.push_back(1);
	}
	//添加哨兵
	raw.insert(raw.begin(), 0);
	for (int k = 0; k < dt.size(); k++)
		_shellInsert(raw, dt[k], fun);
	//撤去哨兵
	raw = std::vector<T>(raw.begin() + 1, raw.end());
}

template<typename T>
void InternalSort<T>::_shellInsert(std::vector<T> &raw, int dleta, InternalSort::compare_function fun) {
	int i, j;
	for (i = dleta + 1; i < raw.size(); i++) {
		//需要将raw[i]插入有序子表
		if (fun(raw[i], raw[i - dleta])) {
			raw[0] = raw[i];
			//找到raw[i]在子表中的插入位置
			for (j = i - dleta; j > 0 && fun(raw[0], raw[j]); j -= dleta)
				raw[j + dleta] = raw[j];
			//插入
			raw[j + dleta] = raw[0];
		}
	}
}

template<typename T>
void InternalSort<T>::bubble(std::vector<T> &raw, InternalSort::compare_function fun) {
	for (int i = 0; i < raw.size(); i++) {
		for (int j = 0; j < (raw.size() - i - 1); j++) {
			if (fun(raw[j + 1], raw[j])) {
				_swap(raw[j], raw[j + 1]);
			}
		} // for (int j = 0; j < (raw.size() - i - 1); j++)
	} // for (int i = 0; i < raw.size(); i++)
}

template<typename T>
void InternalSort<T>::_swap(T &a, T &b) {
	T temp = a;
	a = b;
	b = temp;
}

template<typename T>
void InternalSort<T>::_quick(std::vector<T> &raw, int low, int high, InternalSort::compare_function fun) {
//    std::cout << low << ", " << high << std::endl;
	if (low < high) {
		int pivot_index = _pivotIndex(raw, low, high, fun);
		//枢轴低端排序
		_quick(raw, low, pivot_index - 1, fun);
		//枢轴高端排序
		_quick(raw, pivot_index + 1, high, fun);
	}
}

template<typename T>
int InternalSort<T>::_pivotIndex(std::vector<T> &raw, int low, int high, InternalSort::compare_function fun) {
	T pivot_key = raw[low]; //枢轴记录关键字
	while (low < high) {
		//将比枢轴记录小的记录交换到枢轴低端
		//添加‘==’处理，防止因fun函数未定义'=='情况而产生循环赋值
		while ((low < high) && (fun(pivot_key, raw[high]) || (pivot_key == raw[high]))) {
			high--;
		}
		raw[low] = raw[high];
		//将比枢轴记录大的记录交换到枢轴高端
		while ((low < high) && fun(raw[low], pivot_key)) {
			low++;
		}
		raw[high] = raw[low];
	}
	raw[low] = pivot_key;
	return low;
}

template<typename T>
void InternalSort<T>::simpleSelection(std::vector<T> &raw, InternalSort::compare_function fun) {
	for (int i = 0; i < raw.size(); i++) {
		//选择[i...raw.length]中最小的记录，并交换到raw[i]
		T temp = raw[i];
		int temp_index = i;
		for (int j = i + 1; j < raw.size(); j++) {
			if (fun(raw[j], temp)) {
				temp = raw[j];
				temp_index = j;
			}
		}
		if (temp_index != i)
			_swap(raw[i], raw[temp_index]);
	} // for (int i = 0; i < raw.size(); i++)
}

template<typename T>
void InternalSort<T>::heap(std::vector<T> &raw, InternalSort::compare_function fun) {
	//插入填充元素，使得raw元素下标从1开始
	raw.insert(raw.begin(), raw[0]);
	int len = raw.size() - 1;
	//把raw构建为大/小顶堆
	for (int i = len / 2; i >= 1; i--) {
		_heapAdjust(raw, i, len, fun);
	}
	for (int i = len; i > 1; i--) {
		//交换堆顶元素和未经排序的raw[0, i]中的最后一个记录
		_swap(raw[i], raw[1]);
		_heapAdjust(raw, 1, i - 1, fun);
	}
	//撤去填充元素
	raw = std::vector<T>(raw.begin() + 1, raw.end());
}

template<typename T>
void InternalSort<T>::_heapAdjust(std::vector<T> &heap, int low, int high, InternalSort::compare_function fun) {
	T temp = heap[low];
	for (int j = 2 * low; j <= high; j *= 2) {
		//令j指向low节点左右子树中根节点较大的子树
		if ((j < high) && fun(heap[j], heap[j + 1])) j++;
		//插入heap[low]到合适位置
		if (fun(heap[j], temp)) break;
		heap[low] = heap[j];
		low = j;
	}
	heap[low] = temp;
}

template<typename T>
void InternalSort<T>::mergeSort(std::vector<T> &raw, InternalSort::compare_function fun) {
	std::vector<T> aux_array(raw.size());
	_mergeSort(raw, aux_array, 0, raw.size() - 1, fun);
}

template<typename T>
void InternalSort<T>::_mergeSort(std::vector<T> &raw, std::vector<T> &aux_array, int low, int high,
                                 InternalSort::compare_function fun) {
	int mid = 0;
	if (low < high) {
		mid = (high + low) / 2;
		_mergeSort(raw, aux_array, low, mid, fun);  //左侧数据递归排序
		_mergeSort(raw, aux_array, mid + 1, high, fun);   //右侧数据递归排序
		_mergeArray(raw, aux_array, low, mid, high, fun);   //合并
	}
}

template<typename T>
void InternalSort<T>::_mergeArray(std::vector<T> &raw, std::vector<T> &aux_arr, \
                                  const int low, const int mid, const int high, InternalSort::compare_function fun) {
	int i = low, j = mid + 1, k = 0;
	while ((i <= mid) && (j <= high)) {
		if (fun(raw[i], raw[j])) {
			aux_arr[k++] = raw[i++];
		} else {
			aux_arr[k++] = raw[j++];
		}
	}
	while (i <= mid) {
		aux_arr[k++] = raw[i++];
	}
	while (j <= high) {
		aux_arr[k++] = raw[j++];
	}
	for (k = 0, i = low; i <= high; i++, k++) {
		raw[i] = aux_arr[k];
	}
}

template<typename T>
void InternalSort<T>::mergeSort2(std::vector<T> &raw, InternalSort::compare_function fun) {
	int raw_len = raw.size();
	std::vector<T> aux_arr(raw_len);   //分配辅助数组
	int step, aux_index, left_lower, left_upper, right_lower, right_upper;
	for (step = 1; step < raw_len; step *= 2) {
		for (left_lower = 0; left_lower < raw_len - step; left_lower = right_upper) {
			right_lower = left_upper = left_lower + step;
			right_upper = right_lower + step;
			if (right_upper > raw_len) {
				right_upper = raw_len;
			}
			//归并排序
			aux_index = 0;
			while ((left_lower < left_upper) && (right_lower < right_upper)) {
				if (fun(raw[left_lower], raw[right_lower])) {
					aux_arr[aux_index++] = raw[left_lower++];
				} else {
					aux_arr[aux_index++] = raw[right_lower++];
				}
			}
			//归并循环结束时，可能存在左边数据还有剩余或者右边数据还有剩余
			//若是左边数据还有剩余，则需将剩余数据接回数组
			//若右边数据还有剩余，则不需要做任何操作
			while (left_lower < left_upper) {
				raw[--right_lower] = raw[--left_upper];
			}
			//将排好序的数据存回数组raw
			while (aux_index > 0) {
				raw[--right_lower] = aux_arr[--aux_index];
			}
		} // for(left_lower = 0; left_lower < raw_len-step; left_lower = right_upper)
	} // for(step = 1; step < raw_len; step *= 2)
}

#endif //INTERNAL_SORT_H

