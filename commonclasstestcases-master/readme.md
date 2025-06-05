# 普通班功能测试用例

## IR编译执行器说明

建议在Ubuntu 22.04使用IRCompiler文件夹的程序对自定义IR进行编译与运行。

也可以支持在Ubuntu 20.04上运行，运行方式一样。个别同学如需在Mac系统上运行，请联系指导老师。

假定输入的IR文件名为test.ir，C源文件为test.c。

### C程序到自定义IR

./IRCompiler/Linux-x86_64/Ubuntu-2204/IRCompiler -S -I -o 000_main.ir function/000_main.c

这里提供的IRCompiler可实现MiniC编译器的功能，其结果可作为所开发Minic编译器输出的参考，但不一定要完全一致。

### IR直接执行

./IRCompiler/Linux-x86_64/Ubuntu-2204/IRCompiler -R 000_main.ir

运行后可通过echo $?来获取main函数的返回值。

请注意main函数的返回值为int类型，但shell的%?只能看到8位无符号值。

如果测试用例代码要求有输入，则可通过如下的方式来运行。

```shell
# C语言翻译成中间线性IR
./IRCompiler/Linux-x86_64/Ubuntu-2204/IRCompiler -S -I -o 046_op_priority4.ir function/046_op_priority4.c
# 运行并接收输入数据
./IRCompiler/Linux-x86_64/Ubuntu-2204/IRCompiler -R 046_op_priority4.ir < function/046_op_priority4.in
echo $?
```

## 测试用例

这里提供了139个测试用例(可能会增加或删除)，在function文件夹下。

这里面没有const和数组变量初始化相关的用例。

### minicrun.sh使用

在运行前需要把你开发的minic编译器程序minic拷贝到Git克隆的文件夹下，保证与minicrun.sh在同一个文件夹下。

功能：先运行minic程序得出翻译后的IR，然后通过IRCompiler下提供的工具实现把Ir文件翻译并执行

命令：bash minicrun.sh 开始用例号 结束用例号

从开始用例号开始到结束用例号之间的所有用例进行编译执行

只运行1个用例，例如运行第0个用例，命令如下：

```shell
bash minicrun.sh 0
```

运行一个区间的用例，例如运行从第3个到第10个用例，命令如下：

```shell
bash minicrun.sh 3 10
```

运行全部用例，有两种方式可选择，具体命令如下：

```shell
# 方式一：指定区间
bash minicrun.sh 0 138
# 方式二：不指定参数
bash minicrun.sh
```

## 错误问题调查方法

如果运行出现问题，可按如下的步骤进行调查，假定你输出的IR名字为test.ir

1）利用IRCompiler工具把test.ir转换为test2.ir，看是否成功？

IRCompiler -S -I -o test2.ir test.ir

IRCompiler编译器会对test.ir进行编译检查错误，输出到test2.ir文件中。

请注意变量名可能会重新编号，注意与原始文件的不同。

2）把test2.ir转换成llvm格式的IR，看是否成功

IRCompiler -S -L -o test2.ll test2.ir

当然，也可以由把test2.ir修改成test.ir直接翻译成llvm格式的IR。

3）对llvm格式的IR进行编译，看是否错误

llc test2.ll

根据错误报告的行号定位llvm IR错误的位置，反推到test2.ir的错误地方。
