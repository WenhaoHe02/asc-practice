import torch
import pandas as pd
from torch.utils.data import TensorDataset, DataLoader
from sklearn.model_selection import train_test_split
from sklearn.preprocessing import StandardScaler

df = pd.read_csv('titanic/train.csv')

# 补全数据
print(df.isnull().sum())
df['Age'] = df['Age'].fillna(df['Age'].mean())
df['Embarked'] = df['Embarked'].fillna(df['Embarked'].mode()[0])
print('\n', df.isnull().sum())

# 选一些特征，并且转化成可处理数据， 姓名啥的不符合常理，就不用选了 港口使用one-hot编码方式
df['Sex'] = df['Sex'].map({'male': 0, 'female': 1})
df = pd.get_dummies(df, columns=['Embarked'], drop_first=True)

# 特征列
features = ['Pclass', 'Sex', 'Age', 'SibSp', 'Parch', 'Fare', 'Embarked_Q', 'Embarked_S']

batch_size = 64

# 确保所有特征列都是数值型数据，加入数据标准化
scaler = StandardScaler()
input = scaler.fit_transform(df[features].astype(float))
survived = df['Survived'].values

# 将数据集拆分为训练集和测试集，80% 用于训练，20% 用于测试
input_train, input_test, survived_train, survived_test = train_test_split(input, survived, test_size=0.2, random_state=42)

input_tensor_train = torch.tensor(input_train, dtype=torch.float32)
survived_tensor_train = torch.tensor(survived_train, dtype=torch.float32)
input_tensor_test = torch.tensor(input_test, dtype=torch.float32)
survived_tensor_test = torch.tensor(survived_test, dtype=torch.float32)

train_dataset = TensorDataset(input_tensor_train, survived_tensor_train)
test_dataset = TensorDataset(input_tensor_test, survived_tensor_test)

train_loader = DataLoader(train_dataset, shuffle=True, batch_size=batch_size)
test_loader = DataLoader(test_dataset, shuffle=False, batch_size=batch_size)

class Model(torch.nn.Module):
    def __init__(self):
        super(Model, self).__init__()
        self.l1 = torch.nn.Linear(8, 16)
        self.l2 = torch.nn.Linear(16, 8)
        self.l3 = torch.nn.Linear(8, 4)
        self.l4 = torch.nn.Linear(4, 1)
        self.activate = torch.nn.Sigmoid()

    def forward(self, x):
        x = self.activate(self.l1(x))
        x = self.activate(self.l2(x))
        x = self.activate(self.l3(x))
        x = self.activate(self.l4(x))
        return x

model = Model()

criterion = torch.nn.BCELoss(reduction='sum')
optimizer = torch.optim.Adam(model.parameters(), lr=0.01)  # 使用Adam优化器，学习率设置为0.01

def train_one_epoch():
    running_loss = 0.0
    for batch_idx, data in enumerate(train_loader, 0):
        input, label = data
        label = label.view(-1, 1)  # 调整标签形状匹配 [batch_size, 1]
        y_pred = model(input)
        loss = criterion(y_pred, label)
        running_loss += loss.item()
        optimizer.zero_grad()
        loss.backward()
        optimizer.step()
    return running_loss


def test():
    total = 0
    correct = 0
    with torch.no_grad():
        for data in test_loader:
            input_test, label_test = data
            output = model(input_test)

            predicted = (output >= 0.5).float().view(-1)  # 调整 predicted 的形状为 [batch_size]

            total += label_test.size(0)  # 累加总的样本数
            correct += (predicted == label_test).sum().item()  # 计算正确预测的个数

    # 计算准确率
    accuracy = 100 * correct / total
    print(f'The accuracy of the test set is: {accuracy:.2f} %')


df_test = pd.read_csv('titanic/test.csv')

# 补全测试集中的缺失值并进行相同的预处理
df_test['Age'] = df_test['Age'].fillna(df_test['Age'].mean())
df_test['Fare'] = df_test['Fare'].fillna(df_test['Fare'].mean())  # 测试集中可能有缺失的 'Fare'
df_test['Sex'] = df_test['Sex'].map({'male': 0, 'female': 1})
df_test = pd.get_dummies(df_test, columns=['Embarked'], drop_first=True)

# 现在提取特征列
X_test = scaler.transform(df_test[features].astype(float))  # 使用之前训练时的scaler进行标准化
X_test_tensor = torch.tensor(X_test, dtype=torch.float32)

# 预测函数


if __name__ == '__main__':
    for epoch in range(1000):
        train_one_epoch()
        print("epoch", epoch, '\n')
    test()

def predict():
    model.eval()
    with torch.no_grad():
        predictions = model(X_test_tensor)
        predictions = (predictions >= 0.5).float()
        return predictions.squeeze().numpy()

predicted_labels = predict()

# Kaggle 要求的格式：输出 PassengerId 和 Survived 列
submission = pd.DataFrame({
    'PassengerId': df_test['PassengerId'],
    'Survived': predicted_labels.astype(int)
})

submission.to_csv('submission.csv', index=False)

print("预测结果已保存到 submission.csv 文件。")