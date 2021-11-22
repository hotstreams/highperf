#pragma once

#include <memory>
#include <vector>
#include <functional>

namespace lab1 {
    template <typename Key>
    class Tree final {
    private:
        std::unique_ptr<Tree> left;
        std::unique_ptr<Tree> right;
        Key key;
    public:
        explicit Tree(const Key& _key) noexcept
            : key {_key} {
        }

        Tree(std::initializer_list<Key> list)
            : key {*list.begin()} {
            for (auto it = list.begin() + 1; it != list.end(); ++it) {
                insert(*it);
            }
        }

        Tree(typename std::vector<Key>::iterator begin, typename std::vector<Key>::iterator end)
                : key {*begin} {
            for (auto it = begin + 1; it != end; ++it) {
                insert(*it);
            }
        }

        explicit Tree(const std::vector<Key>& keys)
            : key {keys.at(0)} {
            for (auto it = keys.begin() + 1; it != keys.end(); ++it) {
                insert(*it);
            }
        }

        ~Tree() = default;

        Tree(const Tree&) = delete;
        Tree(Tree&&) noexcept = default;

        Tree& operator=(const Tree&) = delete;
        Tree& operator=(Tree&&) = delete;

        void insert(Key k) {
            if (k < *this) {
                if (!left) {
                    left = std::make_unique<Tree<Key>>(k);
                } else {
                    left->insert(k);
                }
            } else {
                if (!right) {
                    right = std::make_unique<Tree<Key>>(k);
                } else {
                    right->insert(k);
                }
            }
        }

        void insert(const Tree& tree) {
            tree.visit([&] (const auto& k) { insert(k); });
        }

        operator Key&() noexcept { return key; }
        operator const Key&() const noexcept { return key; }

        bool operator<(const Key& rhs) const noexcept {
            return key < rhs;
        }

        bool operator<(const Tree& rhs) const noexcept {
            return key < rhs.key;
        }

        void visit(const std::function<void(const Key&)>& lambda) const noexcept {
            if (left) {
                left->visit(lambda);
            }

            lambda(*this);

            if (right) {
                right->visit(lambda);
            }
        }
    };

    template <typename Value, template <typename> class SequenceContainer>
    void tree_sort(SequenceContainer<Value>& container, std::size_t start, std::size_t end) {
        auto root = Tree<Value>(container.begin() + start, container.begin() + end);
        std::size_t i = start;
        root.visit([&] (const auto& k) { container[i++] = k; });
    }

    template <typename Value, template <typename> class SequenceContainer>
    Tree<Value> tree_sort_split(SequenceContainer<Value>& container, std::size_t start, std::size_t end) {
        return Tree<Value>(container.begin() + start, container.begin() + end);
    }

    template <typename Value, template <typename> class SequenceContainer>
    void tree_sort_omp(SequenceContainer<Value>& container) {
        std::vector<Tree<Value>> trees;
        {
            lab1::Measurer time {"tree sort split"};
            #pragma omp parallel
            {
                auto count = omp_get_num_threads();
                auto current = omp_get_thread_num();
                trees.emplace_back(tree_sort_split(container, container.size() * current / count, container.size() * (current + 1) / count));
            }
        }

        {
            lab1::Measurer time {"tree sort merge"};
            for (int i = 1; i < trees.size(); ++i) {
                trees.front().insert(trees[i]);
            }

            std::size_t i = 0;
            trees.front().visit([&](const auto& k) { container[i++] = k; });
        }
    }
}