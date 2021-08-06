extern crate termion;

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

    use std::{collections::HashMap, convert::TryFrom};

    pub enum Class {
        Pawn,
        Rook,
        Knight,
        Bishop,
        Queen,
        King,
    }

    #[derive(Copy, Clone, PartialEq)]
    pub enum Color {
        Black,
        White,
    }

    pub struct Piece {
        class: Class,
        color: Color,
        index: u8,
        is_dead: bool,
    }

    pub mod hex_point {
        use std::convert::TryFrom;
        use std::ops::{Add, Sub};

        #[derive(Default, Copy, Clone, PartialEq)]
        pub struct HexPoint(i32, i32);

        impl TryFrom<&str> for HexPoint {
            type Error = &'static str;

            fn try_from(value: &str) -> Result<Self, Self::Error> {
                let mut bytes = value.bytes();
                let file = bytes.next().unwrap();
                if file < b'a' && file > b'k' {
                    return Err("Chess Notation: File not parsed properly");
                }
                let maybe_rank: Vec<u8> = bytes.collect();
                let rank: i32 = std::str::from_utf8(maybe_rank.as_slice())
                    .unwrap()
                    .parse()
                    .unwrap();
                if rank < 0 || rank > 11 {
                    return Err("Chess Notation: Rank not parsed properly");
                }
                Ok(HexPoint(i32::from(file - b'a'), rank))
            }
        }

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
        pub const L_UP: HexPoint = HexPoint(-1, 0);
        pub const L_DOWN: HexPoint = HexPoint(-1, -1);
        pub const R_UP: HexPoint = HexPoint(1, 1);
        pub const R_DOWN: HexPoint = HexPoint(1, 0);
    }

    const DEFAULT_START_POSITIONS: [[&str; 18]; 2] = [
        [
            "Pb1", "Pc2", "Pd3", "Pe4", "Pf5", "Pg5", "Ph5", "Pi5", "Pj5", "Rc1", "Ri4", "Nd1",
            "Nh3", "Bf1", "Bf2", "Bf3", "Qe1", "Kg1",
        ],
        [
            "Pb1", "Pc2", "Pd3", "Pe4", "Pf5", "Pg5", "Ph5", "Pi5", "Pj5", "Rc1", "Ri4", "Nd1",
            "Nh3", "Bf1", "Bf2", "Bf3", "Qe1", "Kg1",
        ],
    ];

    fn parse_AN(an: &str) -> Result<(Class, hex_point::HexPoint), &str > {
        let mut iter = an.chars();
        let class = match iter.next().unwrap() {
            'P' => Class::Pawn,
            'R' => Class::Rook,
            'N' => Class::Knight,
            'B' => Class::Bishop,
            'Q' => Class::Queen,
            'K' => Class::King,
            _ => return Err("Parse Failed: Wrong piece notation"),
        };
        let pos = hex_point::HexPoint::try_from(iter.as_str());
        if pos.is_err() {
            return Err(pos.err().unwrap())
        }

        Ok((class, pos.unwrap()))
    }

    struct HexBoard {
        board: HashMap<hex_point::HexPoint, u8>,
    }

    impl HexBoard {}

    pub fn run_chess() {}
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

fn write_hexagons<W: IoWrite>(screen: &mut W) {
    unsafe {
        write!(screen, "{}", HEXAGONS).unwrap();
    }
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
