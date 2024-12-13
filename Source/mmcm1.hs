import System.IO
import System.Directory

import Data.List (foldl')
import Control.Monad (replicateM)

import Debug.Trace (trace, traceShow)
import Text.Printf

import Data.Ratio

-- {-# LANGUAGE ExtendedDefaultRules #-}

-- import Graphics.Matplotlib

type T_ = Double

type Matrix = [[T_]]
type Vector = [T_]

read_matrix :: Int -> IO Matrix
read_matrix rows = replicateM rows $ do map read . words <$> getLine

readDouble :: String -> T_
readDouble = read

lines2words :: [String] -> [String]
lines2words [] = []
lines2words (x:xs) = words x ++ lines2words xs

readLines :: String -> IO [String]
readLines = fmap (lines2words . lines) . readFile

readFileAs :: String -> (String -> a) -> IO [a]
readFileAs file readFoo = do
    coefficients_fromfile <- readLines file
    let coefficients = map readFoo coefficients_fromfile
    return coefficients

vec2mtx :: Matrix -> Vector
vec2mtx x = case x of
    [[]] -> []
    [x:xs] -> [x] ++ vec2mtx [xs]

transpose :: Matrix -> Matrix
transpose ([]:_) = []
transpose x = map head x : transpose (map tail x)

mmult :: Matrix -> Matrix -> Matrix
mmult a b = [[ sum $ zipWith (*) ar bc | bc <- (transpose b)] | ar <- a ]

mat_vec_mul :: Matrix -> Vector -> Vector
mat_vec_mul m v = map (sum . zipWith (*) v) m

identity_mtx :: Int -> Int -> Matrix
identity_mtx n m = [ [if i == j then 1 else 0 | j <- [0 .. m - 1]] | i <- [0 .. n - 1]]

replace_rows :: Matrix -> Int -> Vector -> Int -> Vector -> Matrix
replace_rows m j rowJ i rowI =
  take j m ++ [rowJ] ++ take (i - j - 1) (drop (j + 1) m) ++ [rowI] ++ drop (i + 1) m

hypot :: T_ -> T_ -> T_
hypot a b = sqrt (a * a + b * b)

givens_rotation :: T_ -> T_ -> (T_, T_)
givens_rotation a b =
    let r = hypot a b
        c = a / r
        s = -b / r
    in (c, s)

apply_givens :: (T_, T_) -> Vector -> Vector -> (Vector, Vector)
apply_givens (c, s) rowI rowJ =
    let rowI' = zipWith (\x y -> c * x - s * y) rowI rowJ
        rowJ' = zipWith (\x y -> s * x + c * y) rowI rowJ
    in
    -- trace (printf "c = %f; s = %f" c s) $
    -- trace ("rowI = " ++ show rowI) $
    -- trace ("rowJ = " ++ show rowJ) $
    (rowI', rowJ')

qrd :: Matrix -> (Matrix, Matrix)
qrd mat = foldl' step (identity_mtx n n, mat) [(i, j) | j <- [0 .. m - 1], i <- [j + 1 .. n - 1]]
    where
        n = length mat
        m = length $ head mat
        step (q, r) (i, j) =
            let (c, s) = givens_rotation (r !! j !! j) (r !! i !! j)
                (rj, ri) = apply_givens (c, s) (r !! j) (r !! i)
                r' = replace_rows r j rj i ri
                (qj, qi) = apply_givens (c, s) (q !! j) (q !! i)
                q' = replace_rows q j qj i qi
            in
            -- trace (printf "Iteration i[%d] j[%d]:" i j) $
            -- trace (printf "a[%f] b[%f]:" (r !! j !! j) (r !! i !! j)) $
            -- trace ("mat = " ++ show mat) $
            -- trace ("q = " ++ show q) $
            -- trace ("r = " ++ show r) $
            -- trace ("n = " ++ show n) $
            -- trace ("m = " ++ show m) $
            -- trace (printf "c = %f; s = %f" c s) $
            -- trace ("rj = " ++ show rj) $
            -- trace ("ri = " ++ show ri) $
            -- trace ("r' = " ++ show r') $
            -- trace ("qj = " ++ show qj) $
            -- trace ("qi = " ++ show qi) $
            -- trace ("q' = " ++ show q') $
            -- trace ("-----------------") $
            (q', r')

back_subst :: Matrix -> Vector -> Vector
back_subst r b = foldl' step [] (zip [n - 1, n - 2 .. 0] (drop (m - n) $ reverse r))
    where
        m = length r
        n = length $ head r
        step acc (i, row) =
            let s = sum (zipWith (*) acc (drop (i + 1) row))
                xi = (b !! i - s) / (row !! i)
            in
            -- trace (printf "Iteration %d:\nxi = %f" i xi) $
            -- trace ("n = " ++ show n) $
            -- trace ("acc = " ++ show acc) $
            -- trace (printf "s = %f" s) $
            -- trace ("R = " ++ show r) $
            -- trace ("row = " ++ show row) $
            -- trace ("b !! i = " ++ show (b !! i)) $
            -- trace ("row !! i = " ++ show (row !! i)) $
            -- trace ("-----------------") $
            xi:acc

solve_linear_system :: Matrix -> Vector -> Vector
solve_linear_system a b =
    let (q, r) = qrd a
        -- a_check = mmult (transpose q) r
        qt_b = mat_vec_mul q b
    in
    -- trace ("a_check = " ++ show a_check) $
    back_subst r qt_b

calc_row :: (T_, Int) -> Vector
calc_row (x, 0) = []
calc_row (x, y) = calc_row (x, (y - 1)) ++ [x ^ y]

construct_row :: (T_, Int) -> Vector -> Vector
construct_row (x, y) v = v ++ calc_row (x, y)

concat_tuples :: (Matrix, Matrix) -> (Matrix, Matrix) -> (Matrix, Matrix)
concat_tuples (x, y) (t, s) = (x ++ t, y ++ s)

construct_mtxs :: (Vector, Int) -> (Matrix, Matrix)
construct_mtxs ([x, y], n) = ([construct_row (x, n) []], [[y]])
construct_mtxs ((x:y:xs), n) = concat_tuples head tail
    where
        head = ([construct_row (x, n) []], [[y]])
        tail = (construct_mtxs (xs, n))

-- pick_mtx :: (Matrix, Vector) -> [(T_, T_)]
-- pick_mtx [x:xs] [y:ys] = [(head x, y)] ++ pick_mtx [xs] [ys]

main :: IO ()
main = do
    let file_path = "./Data/mmcm1/data_3_small.txt"
    putStrLn $ "Current file: " ++ file_path
    contents <- readFileAs file_path readDouble
    -- handle <- openFile file_path ReadMode
    -- contents <- hGetContents handle
    -- putStrLn $ show contents
    -- hClose handle
    let (a, b) = construct_mtxs (contents, (div (length contents) 2) - 2)
    -- putStrLn $ "contents: " ++ show contents
    printf "a size = %d; a[0] size = %d; b size = %d\n" (length a) (length $ head a) (length b)
    let b_part = vec2mtx . transpose $ b
        (q, r) = qrd a
        solution = solve_linear_system a b_part
    -- putStrLn $ "A: " ++ show a
    -- putStrLn $ "b: " ++ show b
    putStrLn $ "Solution: " ++ show solution