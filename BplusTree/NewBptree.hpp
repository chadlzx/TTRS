#include <iostream>
#include <cmath>
#include <cstdio>
#include <cstring>
#include <deque>
#include <cassert>
#include "./utility.hpp"
#include "./exceptions.hpp"
#include "./alloc.h"

#ifndef BPTREE_HPP
#define BPTREE_HPP

namespace sjtu {
    /*
     *  Attention : In this file I use std::pair & std::map
     *     replace them with sjtu:pair when finishing development.
     */
    template<
            class key_t,
            class value_t,
            class Compare = std::less<key_t>,
            size_t node_size = 5
    >
    class bptree{
        typedef sjtu::pair<key_t, value_t> value_type;
        typedef std::deque<value_type> array_t;
    private:
        char * filename;
        char * index_file;
        FILE * file;
        int sz = 0;
        off_t head, tail, root;
        file_alloc alloc;
        const size_t tnode_max;
        const size_t block_max;

        struct node {
            key_t key;
            off_t pos;
            off_t father;
            off_t prev;
            off_t next;
            size_t sz = 0;
            int type; //0 tree node 1 block
            node(key_t key = key_t(),
                 off_t pos = invalid_off,
                 off_t father = invalid_off,
                 off_t prev = invalid_off,
                 off_t next = invalid_off,
                 size_t sz = 0,
                 int type = 0)
                    :key(key), pos(pos), father(father), prev(prev), next(next), sz(sz), type(type) {}
        };

        inline void file_reopen() {
            if (file) fflush(file);
        }

        bool Fewer(key_t a, key_t b, Compare C = Compare()) {
            return C(a, b);
        }

        bool Equal(key_t a, key_t b) {
            return !(Fewer(a, b) || Fewer(b, a));
        }

        inline off_t new_node() {
            sz++;
            save_index();
            return alloc.alloc(node_size);
        }

        inline void load_index() {
            alloc.load(index_file);
        }
        inline void save_index() {
            alloc.dump(index_file);
        }

        inline void save_info() {
            fseek(file, 0, SEEK_SET);
            fwrite(&head, sizeof(off_t), 1, file);
            fwrite(&tail, sizeof(off_t), 1, file);
            fwrite(&root, sizeof(off_t), 1, file);
            fflush(file);
            save_index();
        }
        inline void read_info() {
            fseek(file, 0, SEEK_SET);
            fread(&head, sizeof(off_t), 1, file);
            fread(&tail, sizeof(off_t), 1, file);
            fread(&root, sizeof(off_t), 1, file);
        }

        node read_node(off_t p) {
            node r;
            fseek(file, p, SEEK_SET);
            fread(&r, sizeof(node), 1, file);
            return r;
        }

        inline void save_node(const node &p) {
            fseek(file, p.pos, SEEK_SET);
            fwrite(&p, sizeof(node), 1, file);
            file_reopen();
        }
    public:
        bptree(const char * fname, const char * index_fname) :
                tnode_max((node_size - sizeof(node)) / (sizeof(key_t) + sizeof(off_t)) - 1),
                block_max((node_size - sizeof(node)) / (sizeof(key_t) + sizeof(value_t)) - 1)
        {
            file = fopen(fname, "rb+");
            filename = new char[strlen(fname) + 1];
            strcpy(filename, fname);
            index_file = new char[strlen(index_fname) + 1];
            strcpy(index_file, index_fname);
            load_index();
            if (!file) {
                file = fopen(fname, "wb+");
                init();
            }
            else {
                read_info();
                //printf("%lld\n", root);
            }
        }
        ~bptree() {
            save_index();
            save_info();
            //printf("##%lld\n", root);
            if (file) fclose(file);
            //printf("%lld\n", root);
            delete filename;
            delete index_file;
        }


        void init() {
            alloc.clear();
            save_index();
            new_node();
            head = tail = root = invalid_off;
            fseek(file, 0, SEEK_SET);
            save_info();
        }

    };


}

#endif