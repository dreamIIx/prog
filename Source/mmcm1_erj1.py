import numpy as np
import numpy.linalg as la
import matplotlib.pyplot as plt
import math
import time

def lezhandr(epsilon, N):
    lezhandr_p = [1, epsilon]

    for i in range(2, N):
        lezhandr_p.append((2 * i + 1) / (i + 1) * epsilon * lezhandr_p[i - 1] - i / (i + 1) * lezhandr_p[i - 2])
    # for i in range(2, N):
    #     lezhandr_p.append(epsilon**i)

    return lezhandr_p

def householder_reflection_vector(x):
    """
    Вычисляет вектор отражения Хаусхолдера для вектора x.

    Аргументы:
    x -- Вектор (numpy массив), который нужно отразить.

    Возвращает:
    u -- Вектор отражения Хаусхолдера.
    """
    e = np.zeros_like(x)
    e[0] = np.linalg.norm(x)

    # Промежуточный вектор
    u_tilde = x + np.sign(x[0]) * e
    norm_u_tilde = np.linalg.norm(u_tilde)

    if norm_u_tilde == 0:
        return u_tilde  # Если x = 0, возвращаем нулевой вектор

    # Нормируем u_tilde для получения u
    u = u_tilde / norm_u_tilde
    return u

def back_subst(A, b):
    x = np.zeros(len(A[0]))
    for i in range(len(A[0]) - 1, -1, -1):
        x[i] = (b[i] - np.dot(A[i, i + 1:], x[i + 1:])) / A[i, i]
    return x

def forw_subst(L, b):
    n = L.shape[0]
    x = np.zeros(n)
    for i in range(n):
        x[i] = (b[i] - np.dot(L[i, :i], x[:i])) / L[i, i]
    return x

def householder_qr_decomp(A):
    """
    Выполняет QR-разложение матрицы A с использованием оптимизированного метода Хаусхолдера.

    Аргументы:
    A -- Исходная матрица (numpy массив) размерности m x n.

    Возвращает:
    Q -- Ортогональная матрица (numpy массив) размерности m x m.
    R -- Верхнетреугольная матрица (numpy массив) размерности m x n.
    """
    m, n = A.shape
    Q = np.eye(m)
    R = A.copy()

    for k in range(min(m, n)):
        # 1. Выбираем подстолбец R начиная с k-й строки
        x = R[k:, k]

        # 2. Вычисляем вектор отражения Хаусхолдера
        u = householder_reflection_vector(x)

        # 3. Вычисляем блоковое обновление R
        # R[k:, k:] = H_k @ R[k:, k:] = R[k:, k:] - 2 * u @ (u.T @ R[k:, k:])
        R_k = R[k:, k:]
        R[k:, k:] -= 2 * np.outer(u, u @ R_k)

        # 4. Обновляем Q с учетом только текущего подпространства
        Q_k = Q[:, k:]
        Q[:, k:] -= 2 * np.outer(Q_k @ u, u)

    return Q, R

def solve_normal_equations(A, b):
    # QR-разложение с использованием метода Хаусхолдера
    Q, R = householder_qr_decomp(A)

    # A_test = Q @ R

    # Вычисляем Q^T * b
    Qt_b = Q.T @ b

    # print(la.det(Q))
    # print(la.det(R))
    # print(R)

    # Решаем систему Rx = Q^T b для x
    # x = np.linalg.solve(R, Qt_b)
    x = back_subst(R, Qt_b)

    return x

def kholetsky_decomposition(A):
    n = A.shape[0]
    L = np.zeros_like(A)
    for i in range(n):
        for j in range(i + 1):
            if i == j:
                sum_diag = np.sum(L[i, :j]**2)
                L[i, j] = np.sqrt(A[i, i] - sum_diag)
            else:
                sum_off_diag = np.sum(L[i, :j] * L[j, :j])
                L[i, j] = (A[i, j] - sum_off_diag) / L[j, j]
    return L

epsilon = []
sigma = []

file = './Data/mmcm1/data_3.txt'

with open(file, 'r', encoding='utf-8-sig') as f:
    for line in f:
        parts = line.split()
        epsilon.append(float(parts[0]))  # Преобразуем к float для корректных вычислений
        sigma.append(float(parts[1]))    # Преобразуем к float для корректных вычислений

M = len(epsilon)
N = 10

matrix_A = []
for j in range(M):
    row = lezhandr(epsilon[j], N)
    matrix_A.append(row)

matrix_A = np.array(matrix_A)
b = np.array(sigma)  # Преобразуем b в массив NumPy

print(f"Размеры матрицы: {matrix_A.shape}")
print("Количество элементов в epsilon:", len(epsilon))

start_time = time.time()  # время начала выполнения
x = solve_normal_equations(matrix_A, b)
end_time = time.time()  # время окончания выполнения
execution_time = end_time - start_time  # вычисляем время выполнения
print(f"Время выполнения программы: {execution_time} секунд")
print("Решение x:", x)

start_time = time.time()  # время начала выполнения
A_T_A = matrix_A.T @ matrix_A
AT_b = matrix_A.T @ b
# C = kholetsky_decomposition(A_T_A)
# y = forw_subst(C, AT_b)
# x_khol = back_subst(C.T, y)
x_khol = la.solve(A_T_A, AT_b)
end_time = time.time()  # время окончания выполнения
print(x_khol)
execution_time = end_time - start_time  # вычисляем время выполнения
print(f"Время выполнения программы: {execution_time} секунд")
print("norm(x - x_khol) = ", la.norm(x - x_khol))

res_x = [lezhandr(epsilon[i], N) @ x.T for i in range(M)]
res_x_khol = [lezhandr(epsilon[i], N) @ x_khol.T for i in range(M)]
print("norm(actual - x)", la.norm(np.array(sigma) - np.array(res_x)))
print("norm(actual - x_khol)", la.norm(np.array(sigma) - np.array(res_x_khol)))

cond_A = la.cond(matrix_A)
cond_A_T_A = la.cond(A_T_A)
print(cond_A)
print(cond_A_T_A)

sum = 0
for i in range(M):
    sum += abs(lezhandr(epsilon[i], N) @ x.T - sigma[i])**2

NRMSE = 1 / (max(sigma)) * math.sqrt(1/M * sum)
print('NRMSE: ', NRMSE)

plt.plot(epsilon, sigma, color='blue', linewidth=1)
plt.plot(epsilon, res_x, color='red', linewidth=1)
plt.plot(epsilon, res_x_khol, color='green', linewidth=1)
plt.show()
