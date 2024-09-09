# 准备数据集
# 设计模型
# 计算损失以及优化对象
# 写训练周期
import torch

x_data = torch.tensor([[1.0], [2.0], [3.0]]);
y_data = torch.tensor([[4.0], [5.0], [6.0]]);

# 可调用对象举例
class Foobar:
    def __init__(self):
        pass

    def __call__(self, *args, **kwargs):
        print("Hello " + str(args[0]))


foobar = Foobar()
foobar(1, 2, 3)
def func(*args, **kwargs):
    print(args)
    print(kwargs)


func(1, 2, 3, x=4, y=5)


# 设计模型
class LinearModel(torch.nn.Module):
    def __init__(self):
        super(LinearModel, self).__init__()
        self.linear = torch.nn.Linear(1, 1)

    def forward(self, x):
        y_pred = self.linear(x)  # 可调用对象
        return y_pred


model = LinearModel()  # model可以直接调用

# 计算损失以及优化对象
criterion = torch.nn.MSELoss(reduction='sum')
optimizer = torch.optim.SGD(model.parameters(), lr=0.01)

# 写训练周期
for epoch in range(10000):
    y_pred = model(x_data)
    loss = criterion(y_pred, y_data)
    print(epoch, loss)

    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

print('w = ', model.linear.weight.item())
print('b = ', model.linear.bias.item())

x_test = torch.Tensor([[4.0]])
y_test = model(x_test)
print('y_pred = ', y_test.data)

