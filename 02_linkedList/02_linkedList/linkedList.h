#pragma once

// 전방선언
// friend class 설정
template <typename T>
class LinkedList;

template <typename T>
struct Node
{
	T _data;
	Node* _prev;
	Node* _next;
};

template <typename T>
class Iterator
{
public:
	/// 생성자
	Iterator() : _node(nullptr) {};
	Iterator(Node<T>* node) : _node(node) {};
	// 전위 연산자
	Iterator& operator++()
	{
		_node = _node->_next;
		return *this;
	}
	Iterator& operator--()
	{
		_node = _node->_prev;
		return *this;
	}
	// 후위 연산자
	Iterator operator++(int)
	{
		Iterator iterator = *this;
		iterator._node = iterator._node->_next;
		return iterator;
	}
	Iterator operator--(int)
	{
		Iterator iterator = *this;
		iterator._node = iterator._node->_prev;
		return iterator;
	}
	// 참조연산자
	T& operator*()
	{
		// 현재 노드의 데이터 반환
		return _node->_data;
	}
	// 비교연산자
	bool operator==(const Iterator& other)
	{
		return _node == other._node;
	}
	bool operator!=(const Iterator& other)
	{
		return _node != other._node;
	}

private:
	Node<T>* _node;
	friend class LinkedList<T>; // LinkedList 클래스에서 _node 멤버변수에 접근하기 위함
};

template <typename T>
class LinkedList
{
public:
	using iterator = Iterator<T>;

public:
	/// 생성자
	LinkedList()
	{
		_head._next = &_tail;
		_head._prev = nullptr;
		_tail._prev = &_head;
		_tail._next = nullptr;
	}
	/// 소멸자
	~LinkedList()
	{
		clear();
	}
	/// 첫번째 데이터 노드를 가리키는 이터레이터 반환
	iterator begin()
	{
		return iterator(_head._next);
	}
	/// tail 노드를 가리키는 이터레이터 반환
	iterator end()
	{
		return iterator(&_tail);
	}
	/// 데이터 삽입
	void push_front(const T data)
	{
		Node<T>* newNode = new Node<T>;
		Node<T>* nextNode = _head._next;
		// 새로운 노드 생성
		newNode->_data = data;
		newNode->_prev = &_head;
		newNode->_next = nextNode;
		// 기존 노드 변경
		nextNode->_prev = newNode;
		_head._next = newNode;
		// 사이즈 증가
		++_size;
	}
	void push_back(const T data)
	{
		Node<T>* newNode = new Node<T>;
		Node<T>* prevNode = _tail._prev;
		// 새로운 노드 생성
		newNode->_data = data;
		newNode->_prev = prevNode;
		newNode->_next = &_tail;
		// 기존 노드 변경
		prevNode->_next = newNode;
		_tail._prev = newNode;
		// 사이즈 증가
		++_size;
	}
	/// 데이터 삭제
	void pop_front()
	{
		Node<T>* delNode = _head._next;
		Node<T>* nextNode;
		// 삭제하려는 노드가 tail일 경우
		if (delNode == &_tail)
		{
			return;
		}
		nextNode = delNode->_next;
		// 기존 노드 변경
		_head._next = nextNode;
		nextNode->_prev = &_head;
		// 노드 삭제
		delete delNode;
		// 사이즈 감소
		--_size;
	}
	void pop_back()
	{
		Node<T>* delNode = _tail._prev;
		Node<T>* prevNode;
		// 삭제하려는 노드가 head일 경우
		if (delNode == &_head)
		{
			return;
		}
		prevNode = delNode->_prev;
		// 기존 노드 변경
		_tail._prev = prevNode;
		prevNode->_next = &_tail;
		// 노드 삭제
		delete delNode;
		// 사이즈 감소
		--_size;
	}
	/// 전체 삭제
	void clear()
	{
		while (_size > 0)
		{
			pop_back();
		}
	}
	/// 노드 삭제 + 다음 노드 리턴
	iterator erase(iterator iter)
	{
		Node<T>* delNode = iter._node;
		Node<T>* nextNode = delNode->_next;
		Node<T>* prevNode = delNode->_prev;
		// 기존 노드 변경
		prevNode->_next = nextNode;
		nextNode->_prev = prevNode;
		// 노드 삭제
		delete delNode;
		// 사이즈 감소
		--_size;
		// 다음 이터레이터 리턴
		return ++iter;
	}
	/// 데이터를 찾아 노드 삭제
	void remove(T data)
	{
		Iterator<T> iter;
		for (iter = begin(); iter != end(); ++iter)
		{
			if (*iter == data)
			{
				erase(iter);
				break;
			}
		}
	}
	/// 크기 리턴
	int size()
	{
		return _size;
	}
	/// 비어있는지 여부
	bool empty()
	{
		return _size == 0;
	}

private:
	int _size = 0;
	Node<T> _head;
	Node<T> _tail;
};
