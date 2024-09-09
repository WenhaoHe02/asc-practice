import torch.nn
import torchvision
import torch.nn.functional as F
# train_set = torchvision.datasets.MNIST.py(root='../dataset/mnist', train=True, download=True)
# test_set =  torchvision.datasets.MNIST.py(root='../dataset/mnist', train=False, download=True)
x_data = torch.tensor([[1.0], [2.0], [3.0]]);
y_data = torch.tensor([[0.0], [0.0], [1.0]]);
class LogisticRegressionModel(torch.nn.Module):
    def __init__(self):
        super(LogisticRegressionModel, self).__init__()
        self.linear = torch.nn.Linear(1, 1)

    def forward(self, x):
        y_pred = F.sigmoid(self.linear(x))
        return y_pred


model = LogisticRegressionModel()


criterion = torch.nn.BCELoss(reduction='sum')
optimizer = torch.optim.SGD(model.parameters(), lr=0.01)


for epoch in range(10000):
    y_pred = model(x_data)
    loss = criterion(y_pred, y_data)
    print(epoch, loss)

    optimizer.zero_grad()
    loss.backward()
    optimizer.step()

print('w = ', model.linear.weight.item())
print('b = ', model.linear.bias.item())

x_test = torch.Tensor([[3.0]])
y_test = model(x_test)
print('y_pred = ', y_test.data)
