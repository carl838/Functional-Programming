import Data.Time.Clock (getCurrentTime, diffUTCTime)
import Control.DeepSeq (NFData, deepseq)
import GHC.Generics (Generic)

data Queue a = Queue [a] [a] deriving (Show, Generic)
instance NFData a => NFData (Queue a)

emptyQueue :: Queue a
emptyQueue = Queue [] []

enqueue :: a -> Queue a -> Queue a
enqueue x (Queue f r) = Queue f (x : r)

dequeue :: Queue a -> (Maybe a, Queue a)
dequeue (Queue [] []) = (Nothing, emptyQueue)
dequeue (Queue [] r) = dequeue (Queue (reverse r) [])
dequeue (Queue (x:xs) r) = (Just x, Queue xs r)

estimateMemorySize :: Queue a -> Int -> Int -> Int
estimateMemorySize (Queue front back) elementSize overhead =
    (length front + length back) * elementSize + overhead

measureTime :: String -> IO a -> IO a
measureTime description action = do
    start <- getCurrentTime
    result <- action
    end <- getCurrentTime
    putStrLn $ description ++ " took " ++ show (diffUTCTime end start)
    return result

forceEvaluation :: NFData a => a -> IO ()
forceEvaluation x = x `deepseq` return ()

testQueueOperations :: Int -> IO ()
testQueueOperations numElements = do
    let elementSize = 8 
    let overhead = 0   
    putStrLn $ "\nTesting " ++ show numElements ++ " elements:"
    measureTime "Enqueue operation" $ do
        let queue = foldl (\q x -> enqueue x q) emptyQueue [1..numElements]
        forceEvaluation queue
        putStrLn $ "Memory usage after enqueue: " 
                 ++ show (estimateMemorySize queue elementSize overhead) 
                 ++ " bytes"
    measureTime "Dequeue operation" $ do
        let queue = foldl (\q x -> enqueue x q) emptyQueue [1..numElements]
        let finalQueue = foldl (\(_, q) _ -> dequeue q) (Nothing, queue) [1..numElements]
        forceEvaluation (snd finalQueue)

main :: IO ()
main = do
    testQueueOperations 10000
    testQueueOperations 20000
    testQueueOperations 30000
