#pragma once

// ���漱��
// friend class ����
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
	/// ������
	Iterator() : _node(nullptr) {};
	Iterator(Node<T>* node) : _node(node) {};
	// ���� ������
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
	// ���� ������
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
	// ����������
	T& operator*()
	{
		// ���� ����� ������ ��ȯ
		return _node->_data;
	}
	// �񱳿�����
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
	friend class LinkedList<T>; // LinkedList Ŭ�������� _node ��������� �����ϱ� ����
};

template <typename T>
class LinkedList
{
public:
	using iterator = Iterator<T>;

public:
	/// ������
	LinkedList()
	{
		_head._next = &_tail;
		_head._prev = nullptr;
		_tail._prev = &_head;
		_tail._next = nullptr;
	}
	/// �Ҹ���
	~LinkedList()
	{
		clear();
	}
	/// ù��° ������ ��带 ����Ű�� ���ͷ����� ��ȯ
	iterator begin()
	{
		return iterator(_head._next);
	}
	/// tail ��带 ����Ű�� ���ͷ����� ��ȯ
	iterator end()
	{
		return iterator(&_tail);
	}
	/// ������ ����
	void push_front(const T data)
	{
		Node<T>* newNode = new Node<T>;
		Node<T>* nextNode = _head._next;
		// ���ο� ��� ����
		newNode->_data = data;
		newNode->_prev = &_head;
		newNode->_next = nextNode;
		// ���� ��� ����
		nextNode->_prev = newNode;
		_head._next = newNode;
		// ������ ����
		++_size;
	}
	void push_back(const T data)
	{
		Node<T>* newNode = new Node<T>;
		Node<T>* prevNode = _tail._prev;
		// ���ο� ��� ����
		newNode->_data = data;
		newNode->_prev = prevNode;
		newNode->_next = &_tail;
		// ���� ��� ����
		prevNode->_next = newNode;
		_tail._prev = newNode;
		// ������ ����
		++_size;
	}
	/// ������ ����
	void pop_front()
	{
		Node<T>* delNode = _head._next;
		Node<T>* nextNode;
		// �����Ϸ��� ��尡 tail�� ���
		if (delNode == &_tail)
		{
			return;
		}
		nextNode = delNode->_next;
		// ���� ��� ����
		_head._next = nextNode;
		nextNode->_prev = &_head;
		// ��� ����
		delete delNode;
		// ������ ����
		--_size;
	}
	void pop_back()
	{
		Node<T>* delNode = _tail._prev;
		Node<T>* prevNode;
		// �����Ϸ��� ��尡 head�� ���
		if (delNode == &_head)
		{
			return;
		}
		prevNode = delNode->_prev;
		// ���� ��� ����
		_tail._prev = prevNode;
		prevNode->_next = &_tail;
		// ��� ����
		delete delNode;
		// ������ ����
		--_size;
	}
	/// ��ü ����
	void clear()
	{
		while (_size > 0)
		{
			pop_back();
		}
	}
	/// ��� ���� + ���� ��� ����
	iterator erase(iterator iter)
	{
		Node<T>* delNode = iter._node;
		Node<T>* nextNode = delNode->_next;
		Node<T>* prevNode = delNode->_prev;
		// ���� ��� ����
		prevNode->_next = nextNode;
		nextNode->_prev = prevNode;
		// ��� ����
		delete delNode;
		// ������ ����
		--_size;
		// ���� ���ͷ����� ����
		return ++iter;
	}
	/// �����͸� ã�� ��� ����
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
	/// ũ�� ����
	int size()
	{
		return _size;
	}
	/// ����ִ��� ����
	bool empty()
	{
		return _size == 0;
	}

private:
	int _size = 0;
	Node<T> _head;
	Node<T> _tail;
};
