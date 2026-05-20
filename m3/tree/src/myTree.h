#pragma once

#include <iostream>

template <typename T>
class RBTree {
private:
    enum class Color {
        Red,
        Black
    };

    struct Node {
        T data;
        Color color = Color::Black;
        Node* parent = nullptr;
        Node* left = nullptr;
        Node* right = nullptr;

        Node() : data() {}

        explicit Node(const T& value)
            : data(value),
              color(Color::Red),
              parent(nullptr),
              left(nullptr),
              right(nullptr) {
        }
    };

public:
    RBTree()
        : null_(new Node()),
          root_(null_) {
    }

    ~RBTree() {
        Clear(root_);
        delete null_;
    }

    void insert(const T& value);
    void remove(const T& value);

    const T* find(const T& value) const;
    void print() const;

    void PrintToOutput(std::ostream& out) const;

private:
    void Clear(Node* node);

    void LeftRotate(Node* x);
    void RightRotate(Node* y);

    // replacing one subtree with another
    void Transplant(Node* old_node, Node* new_node);

    void DeleteNode(Node* node);

    void InsertFixup(Node* node);
    void RemoveFixup(Node* node, Node* parent);

    Node* FindNode(const T& value) const;
    Node* Minimum(Node* node) const;    

    void PrintTree(Node* node, std::ostream& out) const;

private:
    Node* null_;
    Node* root_;
};


/**********************************
            IMPLEMENTATION
**********************************/

template <typename T>
void RBTree<T>::insert(const T& value) {
    Node* parent = null_;
    Node* current = root_;

    while (current != null_) {
        parent = current;

        if (value < current->data) {
            current = current->left;
        } 
        else if (current->data < value) {
            current = current->right;
        } 
        else {
            return;
        }
    }

    Node* node = new Node(value);
    node->left = null_;
    node->right = null_;
    node->parent = parent;

    if (parent == null_) {
        root_ = node;
    } 
    else if (value < parent->data) {
        parent->left = node;
    } 
    else {
        parent->right = node;
    }

    InsertFixup(node);
}

template <typename T>
void RBTree<T>::remove(const T& value) {
    Node* node = FindNode(value);

    if (node == null_) {
        return;
    }

    DeleteNode(node);
}

template <typename T>
const T* RBTree<T>::find(const T& value) const {
    Node* node = FindNode(value);

    if (node == null_) {
        return nullptr;
    }

    return &node->data;
}

template <typename T>
void RBTree<T>::print() const {
    PrintTree(root_, std::cout);
    std::cout << '\n';
}

template <typename T>
void RBTree<T>::PrintToOutput(std::ostream& out) const {
    PrintTree(root_, out);
    out << '\n';    
}

template <typename T>
void RBTree<T>::Clear(Node* node) {
    if (node == null_) {
        return;
    }

    Clear(node->left);
    Clear(node->right);
    delete node;
}

template <typename T>
void RBTree<T>::LeftRotate(Node* x) {
    if (x == null_ || x->right == null_) {
        return;
    }

    Node* y = x->right;

    x->right = y->left;

    if (y->left != null_) {
        y->left->parent = x;
    }

    y->parent = x->parent;

    if (x->parent == null_) {
        root_ = y;
    } 
    else if (x == x->parent->left) {
        x->parent->left = y;
    }
    else {
        x->parent->right = y;
    }

    y->left = x;
    x->parent = y;
}

template <typename T>
void RBTree<T>::RightRotate(Node* y)  {
    if (y == null_ || y->left == null_) {
        return;
    }

    Node* x = y->left;
    y->left = x->right;

    if (x->right != null_) {
        x->right->parent = y;
    }

    x->parent = y->parent;

    if (y->parent == null_) {
        root_ = x;
    } 
    else if (y == y->parent->left) {
        y->parent->left = x;
    } 
    else {
        y->parent->right = x;
    }

    x->right = y;
    y->parent = x;
}

template <typename T>
void RBTree<T>::Transplant(Node* old_node, Node* new_node) {
    if (old_node->parent == null_) {
        root_ = new_node;
    }
    else if (old_node == old_node->parent->left) {
        old_node->parent->left = new_node;
    } 
    else {
        old_node->parent->right = new_node;
    }

    if (new_node != null_) {
        new_node->parent = old_node->parent;
    }
}

template <typename T>
void RBTree<T>::DeleteNode(Node* node) {
    Node* removed_node = node;
    Color original_color = removed_node->color;

    Node* fixup_node = null_;
    Node* fixup_parent = null_;

    if (node->left == null_) {
        fixup_node = node->right;
        fixup_parent = node->parent;

        Transplant(node, node->right);
    } 
    else if (node->right == null_) {
        fixup_node = node->left;
        fixup_parent = node->parent;

        Transplant(node, node->left);
    } 
    else {
        removed_node = Minimum(node->right);
        original_color = removed_node->color;

        fixup_node = removed_node->right;

        if (removed_node->parent == node) {
            fixup_parent = removed_node;
        } 
        else {
            fixup_parent = removed_node->parent;

            Transplant(removed_node, removed_node->right);

            removed_node->right = node->right;
            removed_node->right->parent = removed_node;
        }

        Transplant(node, removed_node);

        removed_node->left = node->left;
        removed_node->left->parent = removed_node;
        removed_node->color = node->color;
    }

    delete node;

    if (original_color == Color::Black) {
        RemoveFixup(fixup_node, fixup_parent);
    }
}

template <typename T>
void RBTree<T>::InsertFixup(Node* node) {
    while (node->parent->color == Color::Red) {
        if (node->parent == node->parent->parent->left) {
            Node* uncle = node->parent->parent->right;

            if (uncle->color == Color::Red) {
                node->parent->color = Color::Black;
                uncle->color = Color::Black;
                node->parent->parent->color = Color::Red;

                node = node->parent->parent;
            } 
            else {
                if (node == node->parent->right) {
                    node = node->parent;
                    LeftRotate(node);
                }

                node->parent->color = Color::Black;
                node->parent->parent->color = Color::Red;
                RightRotate(node->parent->parent);
            }
        } 
        else {
            Node* uncle = node->parent->parent->left;

            if (uncle->color == Color::Red) {
                node->parent->color = Color::Black;
                uncle->color = Color::Black;
                node->parent->parent->color = Color::Red;

                node = node->parent->parent;
            } 
            else {
                if (node == node->parent->left) {
                    node = node->parent;
                    RightRotate(node);
                }

                node->parent->color = Color::Black;
                node->parent->parent->color = Color::Red;
                LeftRotate(node->parent->parent);
            }
        }
    }

    root_->color = Color::Black;
}

template <typename T>
void RBTree<T>::RemoveFixup(Node* node, Node* parent) {
    while (node != root_ && node->color == Color::Black) {
        if (node == parent->left) {
            Node* brother = parent->right;

            if (brother->color == Color::Red) {
                brother->color = Color::Black;
                parent->color = Color::Red;
                LeftRotate(parent);
                brother = parent->right;
            }

            if (brother->left->color == Color::Black &&
                brother->right->color == Color::Black) {
                brother->color = Color::Red;
                node = parent;
                parent = node->parent;
            } 
            else {
                if (brother->right->color == Color::Black) {
                    if (brother->left != null_) {
                        brother->left->color = Color::Black;
                    }

                    brother->color = Color::Red;
                    RightRotate(brother);
                    brother = parent->right;
                }

                brother->color = parent->color;
                parent->color = Color::Black;

                if (brother->right != null_) {
                    brother->right->color = Color::Black;
                }

                LeftRotate(parent);

                node = root_;
                parent = null_;
            }
        } 
        else {
            Node* brother = parent->left;

            if (brother->color == Color::Red) {
                brother->color = Color::Black;
                parent->color = Color::Red;
                RightRotate(parent);
                brother = parent->left;
            }

            if (brother->right->color == Color::Black &&
                brother->left->color == Color::Black) {
                brother->color = Color::Red;
                node = parent;
                parent = node->parent;
            } 
            else {
                if (brother->left->color == Color::Black) {
                    if (brother->right != null_) {
                        brother->right->color = Color::Black;
                    }

                    brother->color = Color::Red;
                    LeftRotate(brother);
                    brother = parent->left;
                }

                brother->color = parent->color;
                parent->color = Color::Black;

                if (brother->left != null_) {
                    brother->left->color = Color::Black;
                }

                RightRotate(parent);

                node = root_;
                parent = null_;
            }
        }
    }

    if (node != null_) {
        node->color = Color::Black;
    }
}

template <typename T>
RBTree<T>::Node* RBTree<T>::FindNode(const T& value) const {
    Node* current = root_;

    while (current != null_) {
        if (value < current->data) {
            current = current->left;
        } 
        else if (current->data < value) {
            current = current->right;
        } 
        else {
            return current;
        }
    }

    return null_;
}

template <typename T>
RBTree<T>::Node* RBTree<T>::Minimum(Node* node) const {
    while (node->left != null_) {
        node = node->left;
    }

    return node;
}

template <typename T>
void RBTree<T>::PrintTree(Node* node, std::ostream& out) const {
    if (node == null_) {
        return;
    }

    PrintTree(node->left, out);
    out << node->data << ' ';
    PrintTree(node->right, out);
}