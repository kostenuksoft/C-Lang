#pragma once
#include "Framework.hpp"


namespace SLL
{
    
    template<class Ty>
    class List {

    public:
        struct Node {
            explicit Node(const Ty& value) : data(std::move(value)), next(nullptr) {}

            static Node* allocate(Ty value) {
                Node* node = new Node(value);
                return node;
            }

            Ty data;
            Node* next;
        };

        List() : head(nullptr), len(0) {}

	    ~List() {
            clear();
        }

        List(const List& list) = default;
        List& operator=(const List& list) = default;

        List(List&& list) = default;
        List& operator=(List&& list) = default;

        [[nodiscard]] bool invalid() const { return head == nullptr; } 
        [[nodiscard]] bool is_empty() const { return head != nullptr; }
        [[nodiscard]] size_t size() const { return len; }

        [[nodiscard]] Node* get_head() { return head; }

        void insert(const Ty& value, const size_t index) {
	        insert_by(new Node(value), index);
        }
        void insert(const Ty&& value, const size_t index) {
	        insert_by(new Node(std::move(value), index));
        }

        void insert_by(Node* node, const size_t index) {
            if (index == 0) {
                node->next = head;
                head = node;
            }
            else {
                if (index > size()) return;
                Node* prev = get_node(index - 1);
                node->next = prev->next;
                prev->next = node;
            }

            ++len;
        }

        [[nodiscard]] Ty& get(const size_t index) const {
            if (is_empty()) throw std::out_of_range("List is empty");
            return get_node(index)->data;
        }

        void remove(const Node* node)
        {
            if (invalid()) return;

            size_t index = 0;
            Node* temp = head;
            while (temp) {
                if (temp == node) {
                    Node* prev = get_node(index - 1);
                    prev->next = node->next;
                    delete node;
                    --len;
                    break;
                }
                ++index;
                temp = temp->next;
            }
        }
        void remove_by(const size_t index) {
            if (invalid() || index > size() || get_node(index) == nullptr) return;
           
            if (index == 0) {
                this->head = head->next;
                delete head;
                --len;
                return;
            }

            Node* prev = get_node(index - 1);
            Node* to_remove = prev->next;
            prev->next = to_remove->next;
            delete to_remove;
            --len;
        }

        void clear() {
            if (invalid()) return;
            Node* freeme = head;
            while (freeme) {
                Node* next = freeme->next;
                delete freeme;
                freeme = next;
            }
            len = 0;
        }

    private:

        [[nodiscard]] Node* get_node(const size_t index) const {
            if (index > size()) return nullptr;
            Node* temp = head;
            for (size_t i = 0; i < index; ++i) temp = temp->next;
            return temp;
        }

        Node* head;
        size_t len;
    };
}
   




