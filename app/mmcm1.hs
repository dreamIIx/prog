import Data.List (foldl')
import Control.Monad (replicateM)

-- Тип данных для матрицы
type Matrix = [[Double]]
type Vector = [Double]

-- Вычисление гипотенузы (для избежания переполнения)
hypot :: Double -> Double -> Double
hypot a b = sqrt (a*a + b*b)

-- Преобразование Гивенса для зануления элемента матрицы
givensRotation :: Double -> Double -> (Double, Double)
givensRotation a b =
  let r = hypot a b
      c = a / r
      s = -b / r
  in (c, s)

-- Применение матрицы вращения Гивенса к двум строкам матрицы
applyGivens :: (Double, Double) -> Vector -> Vector -> (Vector, Vector)
applyGivens (c, s) rowI rowJ = 
  let rowI' = zipWith (\x y -> c*x - s*y) rowI rowJ
      rowJ' = zipWith (\x y -> s*x + c*y) rowI rowJ
  in (rowI', rowJ')

-- QR-разложение методом Гивенса
qrDecomposition :: Matrix -> (Matrix, Matrix)
qrDecomposition mat = foldl' step (identityMatrix n, mat) [(i, j) | j <- [0..n-1], i <- [j+1..n-1]]
  where
    n = length mat
    step (q, r) (i, j) =
      let (c, s) = givensRotation (r !! j !! j) (r !! i !! j)
          (rowJ', rowI') = applyGivens (c, s) (r !! j) (r !! i)
          r' = replaceRows r j rowJ' i rowI'
          q' = replaceRows q j (fst (applyGivens (c, s) (q !! j) (q !! i))) i (fst (applyGivens (c, s) (q !! j) (q !! i)))
      in (q', r')
    replaceRows m j rowJ i rowI = take j m ++ [rowJ] ++ take (i - j - 1) (drop (j + 1) m) ++ [rowI] ++ drop (i + 1) m

-- Единичная матрица
identityMatrix :: Int -> Matrix
identityMatrix n = [ [if i == j then 1 else 0 | j <- [0..n-1]] | i <- [0..n-1]]

-- Умножение матрицы на вектор
matVecMul :: Matrix -> Vector -> Vector
matVecMul m v = map (sum . zipWith (*) v) m

-- Решение системы уравнений с верхнетреугольной матрицей методом обратной подстановки
backSubstitution :: Matrix -> Vector -> Vector
backSubstitution r b = reverse (foldl' step [] (zip [n-1,n-2..0] r))
  where
    n = length r
    step acc (i, row) = 
      let s = sum (zipWith (*) (reverse acc) (drop (i+1) row))
          xi = (b !! i - s) / (row !! i)
      in xi:acc

-- Основная функция для решения линейной системы Ax = b через QR-разложение
solveLinearSystem :: Matrix -> Vector -> Vector
solveLinearSystem a b =
  let (q, r) = qrDecomposition a
      qt_b = matVecMul (transpose q) b
  in backSubstitution r qt_b

-- Транспонирование матрицы
transpose :: Matrix -> Matrix
transpose ([]:_) = []
transpose x = map head x : transpose (map tail x)

-- -- Пример использования
-- main :: IO ()
-- main = do
--   let a = [[1, 2, 4], [3, 8, 14], [2, 6, 13]]
--       b = [3, 13, 4]
--       solution = solveLinearSystem a b
--   putStrLn $ "Solution: " ++ show solution

readMatrix :: Int -> IO Matrix
readMatrix rows = replicateM rows $ do map read . words <$> getLine

main :: IO ()
main = do
  putStrLn "Enter number of rows:"
  n <- readLn  -- Reading the number of rows
  putStrLn "Enter the matrix (each row on a new line):"
  matrix <- readMatrix n
  putStrLn "The matrix you entered is:"
  mapM_ print matrix
  putStrLn "Enter the b-part (each row on a new line):"
  b_part <- readMatrix n
  putStrLn "The b-part you entered is:"
  mapM_ print matrix
  let solution = solveLinearSystem matrix b_part
  putStrLn $ "Solution: " ++ show solution
