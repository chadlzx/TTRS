# Bptree使用手册
## 类成员说明
1. <code>tailLeaf</code> 和 <code>headLeaf</code>用来维护叶节点首尾的位置，从而便利起来十分方便。
2. <code>children[]</code>, 存儿子的在文件中的地址.
3. <code>data[]</code>, 存数据，index和leaf都需要.
## 迭代器说明
1. <code>find(Key key) </code>返回第一个id为key的迭代器.
2. <code>begin(),end(),++</code>用法比较常规.
## public函数
1. 初始化: <code>Bptree<Key, value> eg("filename")</code>, 你也可以提供新的indexsize&pagesize;
2. <code>eg.at(key)</code> , 随机访问
3. <code>eg.clear() </code> , 删库跑路
4. <code>eg.insert( pair<Key, value> )</code>, 插入
5. <code>eg.erase(Key id)</code>， 删除
6. <code>eg.traverse()</code>, 遍历所有叶子节点 
7. <code>eg.count(Key key)</code>, 数一下id为key的一共有多少个.
## 部分private函数
1. <code>node * t = Getnode(off_t pos)</code> 将文件中的信息存入内存
2. <code>save_node(node * p)</code>和<code>save_main()</code>, 写入单个节点信息和写入b+树主要成员信息