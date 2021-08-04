extern crate termion;

// use std::collections::HashMap;
use std::fmt::Write as FmtWrite;
use std::io::Write as IoWrite;
use std::io::{stdin, stdout};
use termion::event::Key;
use termion::input::TermRead;
use termion::raw::IntoRawMode;
use termion::screen::*;

const N: i32 = 6;
static mut HEXAGONS: String = String::new();

mod chess {

    pub enum Class {
        Pawn(bool),
        Rook,
        Knight,
        Bishop,
        Queen,
        King,
    }

    pub enum Color {
        Black,
        White,
    }

    pub struct Piece {
        class: Class,
        color: Color,
    }

    impl Piece {
        fn get_moves(&self) {
            todo!();
        }
    }

    pub mod hex_point {
        use std::ops::{Add, Sub};

        #[derive(Default, Copy, Clone, PartialEq)]
        pub struct HexPoint(i32, i32);

        impl Add for HexPoint {
            type Output = Self;

            fn add(self, other: Self) -> Self {
                Self(self.0 + other.0, self.1 + other.1)
            }
        }

        impl Sub for HexPoint {
            type Output = Self;

            fn sub(self, other: Self) -> Self {
                Self(self.0 - other.0, self.1 - other.1)
            }
        }

        pub const UP: HexPoint = HexPoint(0, 1);
        pub const DOWN: HexPoint = HexPoint(0, -1);
        pub const L_UP: HexPoint = HexPoint(1, 0);
        pub const L_DOWN: HexPoint = HexPoint(-1, -1);
        pub const R_UP: HexPoint = HexPoint(1, 1);
        pub const R_DOWN: HexPoint = HexPoint(1, 0);
    }
}
fn generate_hexagons() {
    let height = N * 4 - 1;
    let width = N * 8 - 3;
    let mut buf_string = String::new();

    for row in 0..height {
        let (start, end) = {
            let blank = {
                let mid = (height - 1) / 2;
                let row_index = mid - row;
                let res = if row_index > 0 {
                    (width - ((mid - row_index) * 8 + 3)) / 2
                } else {
                    (width - ((mid + row_index) * 8 + 5)) / 2
                };
                if res > 0 {
                    res
                } else {
                    0
                }
            };
            for _ in 0..blank {
                write!(buf_string, " ").unwrap();
            }
            let diff = if (row % 2 == 0) ^ (N % 2 == 0) { 0 } else { 4 };
            (blank + diff, width - blank + diff)
        };
        for col in start..end {
            let modular = col % 8;
            if modular > 0 && modular < 4 {
                write!(buf_string, "_").unwrap();
            } else if modular == 0 {
                write!(buf_string, "\\").unwrap();
            } else if modular == 4 {
                write!(buf_string, "/").unwrap();
            } else {
                write!(buf_string, " ").unwrap();
            }
        }
        write!(buf_string, "\r\n").unwrap();
    }
    unsafe {
        HEXAGONS = buf_string;
    }
}

fn generate_hexagons_large() {
    let height = 8 * N - 3;
    let width = 14 * N - 5;
    let mut buf_string = String::new();

    for i in 0..height {
        let blank = {
            let index = i32::abs(i - (height + 1) / 2);
            if index % 2 == 0 {
                width + 1 - 7 * (index / 2)
            } else {
                width + 7 - 7 * ((index + 1) / 2)
            }
        };

        for j in 0..width {
            let (slash_pos, hex_len) = match (i + 3 * N) % 4 {
                0 => (7, 8),
                1 => (1, 6),
                2 => (0, 8),
                3 => (8, 6),
                _ => panic!("Impossible modular result"),
            };

            let temp = (j - slash_pos + 14) % 14;

            if i32::abs(j + 1 - (width + 1) / 2) > blank {
                write!(buf_string, " ").unwrap();
            } else if hex_len == 8 && temp > hex_len {
                write!(buf_string, "_").unwrap();
            } else if temp == 0 {
                write!(buf_string, "/").unwrap();
            } else if temp == hex_len {
                write!(buf_string, "\\").unwrap();
            } else {
                write!(buf_string, " ").unwrap();
            }
        }
        write!(buf_string, "\r\n").unwrap();
    }

    unsafe {
        HEXAGONS = buf_string;
    }
}

fn write_hexagons<W: IoWrite>(screen: &mut W) {
    unsafe {
        write!(screen, "{}", HEXAGONS).unwrap();
    }
}

fn draw_pieces(/*pieces: &HashMap<>*/) {
    todo!()
}

fn hexpos_to_term(pos: (i32, i32)) -> (i32, i32) {
    todo!();
}

fn main() {
    let stdin = stdin();
    let mut screen = AlternateScreen::from(stdout().into_raw_mode().unwrap());

    write!(
        screen,
        "{}{}{}Welcome To Glinski's hexagonal chess!{}",
        termion::cursor::Hide,
        termion::clear::All,
        termion::cursor::Goto(1, 1),
        termion::cursor::Goto(1, 4)
    )
    .unwrap();
    generate_hexagons();
    write_hexagons(&mut screen);

    screen.flush().unwrap();

    for c in stdin.keys() {
        match c.unwrap() {
            Key::Char('q') => break,
            _ => {}
        }
    }

    write!(screen, "{}", termion::cursor::Show).unwrap();
}
