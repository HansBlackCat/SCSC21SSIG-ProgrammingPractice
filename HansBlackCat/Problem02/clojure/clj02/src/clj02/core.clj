(ns clj02.core)

(require '[org.clojure/core.async :as async])

(defn factor? [a b]
  zero? (mod b a))

(defn foo
  "I don't do a whole lot."
  [x]
  (println x "Hello, World!"))
