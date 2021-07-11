mod primes {
    static mut PRIME_VEC : Vec<u64> = Vec::new();

    pub fn get_nth_prime(n: u64) -> u64 {
        unsafe {
            if PRIME_VEC.is_empty() {
                PRIME_VEC.push(2);
                PRIME_VEC.push(3);
            }
        }

        unsafe {
            let mut candidate = PRIME_VEC.last().unwrap().clone() as u64;
            while PRIME_VEC.len() < n as usize {
                candidate += 1;
                if is_prime_calc(candidate) {
                    PRIME_VEC.push(candidate);
                }
            }
        }

        unsafe { PRIME_VEC[n as usize - 1] }
    }

    pub fn is_prime(n: u64) -> bool {
        if unsafe { n < PRIME_VEC.last().unwrap().clone() } {
            unsafe {
                PRIME_VEC.iter()
                    .any(|&x| x == n)
            }
        } else {
            is_prime_calc(n)
        }
    }

    fn is_prime_calc(n: u64) -> bool {
        let sqrted = f64::sqrt(n as f64).ceil() as u64;
        for i in 2..sqrted + 1 {
            let modulo = n % i;
            if modulo == 0 {
                return false
            }
        }
        true
    }

    pub fn get_nth_fac(n: u64) -> u64 {
        if n == 1u64 {
            return 2u64
        } else if n == 2u64 {
            return 3u64
        }

        let mut i = 2u64;
        let mut fac = 2u64;
        let mut count = 2;
        loop {
            i += 1;
            fac *= i;
            let pair = [fac - 1, fac + 1];
            for p in pair.iter() {
                if is_prime(p.clone()) {
                    count += 1;
                }

                if count == n {
                    return p.clone()
                }
            }
        }
    }

    pub fn get_nth_leyland(n: u64) -> u64 {
        let mut y = 3;
        let mut count = 0;
        loop {
            for x in 2..y {
                let candidate = u64::pow(x, y as u32) + u64::pow(y, x as u32);
                if is_prime(candidate) {
                    count += 1;
                    if count >= n {
                        return candidate
                    }
                }
            }
            y += 1;
        }
    }
}

fn main() {
    assert_eq!(2, primes::get_nth_prime(1));
    assert_eq!(5, primes::get_nth_prime(3));
    assert_eq!(11, primes::get_nth_prime(5));

    for i in 2..5 {
        let num = u64::pow(10, i) + 1;
        let prime = primes::get_nth_prime(num);
        println!("{}st prime: {}", num, prime);
    }

    assert_eq!(5, primes::get_nth_fac(3));
    assert_eq!(7, primes::get_nth_fac(4));

    println!("8th factorial prime: {}", primes::get_nth_fac(8));

    assert_eq!(17, primes::get_nth_leyland(1));
    assert_eq!(593, primes::get_nth_leyland(2));

    println!("4th Leyland prime: {}", primes::get_nth_leyland(4));

}
