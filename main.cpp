#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <expected>
#include <format>
#include <functional>
#include <iostream>
#include <print>
#include <random>
#include <span>
#include <string_view>
#include <utility>

using sz = std::size_t;
using u8 = std::uint8_t;

enum class GameState : u8 {
	XTurn,
	OTurn,
	XWin,
	OWin,
	Draw,
};

class GameBoard {
public:
	enum class CellType : u8 {
		Empty,
		X,
		O,
	};

	static constexpr auto cell_type_to_char(const CellType cell_type) -> char {
		switch (cell_type) {
		case CellType::Empty: return ' ';
		case CellType::X:     return 'X';
		case CellType::O:     return 'O';
		}
		std::unreachable();
	}

	GameBoard()
	{
		reset();
	}

	void reset()
	{
		std::ranges::fill(board, CellType::Empty);
	}

	void draw() const
	{
		std::println("     1   2   3");
		std::println("     ---------");
		const std::array<char, 3> row_chars = {'A', 'B', 'C'};
		for (sz i = 0; i < row_count; i++) {
			const auto row = get_row(i);
			std::println(" {} | {} | {} | {}",
				row_chars[i],
				cell_type_to_char(row[0]),
				cell_type_to_char(row[1]),
				cell_type_to_char(row[2])
			);
			if (i < row_count - 1) {
				std::println("   | ---------");
			}
		}
	}

	[[nodiscard]] auto place(const CellType cell_type, std::string_view target)
		-> std::expected<void, std::string>
	{
		if (target.length() != 2) {
			return std::unexpected{"Invalid coordinate length."};
		}

		const char row_char = target[0];
		const char col_char = target[1];

		const auto maybe_row = std::invoke([row_char] -> std::optional<u8> {
			switch (row_char) {
			case 'A': return 0;
			case 'B': return 1;
			case 'C': return 2;
			}
			return {};
		});
		if (!maybe_row.has_value()) {
			return std::unexpected{
				std::format("'{}' is not a valid row letter.", row_char)
			};
		}
		const u8 row = *maybe_row;

		const auto maybe_col = std::invoke([col_char] -> std::optional<u8> {
			switch (col_char) {
			case '1': return 0;
			case '2': return 1;
			case '3': return 2;
			}
			return {};
		});
		if (!maybe_col.has_value()) {
			return std::unexpected{
				std::format("'{}' is not a valid column number.", col_char)
			};
		}
		const u8 col = *maybe_col;

		auto& cell = get_cell(row, col);
		if (cell != CellType::Empty) {
			return std::unexpected{"That cell is occupied."};
		}

		cell = cell_type;
		return {};
	}

	[[nodiscard]] auto check_for_win() -> std::optional<GameState>
	{
		for (sz i = 0; i < row_count; i++) {
			const auto& row_root = get_cell(i, 0);
			if (row_root != CellType::Empty
				&& row_root == get_cell(i, 1)
				&& row_root == get_cell(i, 2)
		        ) {
				return cell_type_to_game_state(row_root);
			}
			const auto& col_root = get_cell(0, i);
			if (col_root != CellType::Empty
				&& col_root == get_cell(1, i)
				&& col_root == get_cell(2, i)
			) {
				return cell_type_to_game_state(col_root);
			}
		}
		const auto& back_diagonal_root = get_cell(0, 0);
		if (back_diagonal_root != CellType::Empty
			&& back_diagonal_root == get_cell(1, 1)
			&& back_diagonal_root == get_cell(2, 2)
		) {
			return cell_type_to_game_state(back_diagonal_root);
		}
		const auto& forward_diagonal_root = get_cell(2, 0);
		if (forward_diagonal_root != CellType::Empty
			&& forward_diagonal_root == get_cell(1, 1)
			&& forward_diagonal_root == get_cell(0, 2)
		) {
			return cell_type_to_game_state(forward_diagonal_root);
		}

		if (std::ranges::all_of(board, [](auto& cell) { return cell != CellType::Empty; })) {
			return GameState::Draw;
		}

		return {};
	}

private:
	static constexpr sz row_count = 3;
	static constexpr sz column_count = 3;
	std::array<CellType, row_count * column_count> board;

	[[nodiscard]] constexpr auto get_row(const sz index) const -> std::span<const CellType, row_count>
	{
		return std::span<const CellType, row_count>{&board[index * row_count], column_count};
	}

	[[nodiscard]] constexpr auto get_cell(const sz row, const sz col) -> CellType&
	{
		return board[(row * row_count) + col];
	}

	[[nodiscard]] static constexpr auto cell_type_to_game_state(const CellType cell_type) -> GameState
	{
		switch (cell_type) {
		case CellType::X: return GameState::XWin;
		case CellType::O: return GameState::OWin;
		default: std::unreachable();
		}
	}
};

auto main() -> int
{
	GameBoard board;
	GameState state = GameState::XTurn;

	std::string user_input;
	
	std::println();

	while (true) {
		board.draw();
		std::println();

		auto cell_type = std::invoke([state] -> GameBoard::CellType {
			switch (state) {
			case GameState::XTurn: return GameBoard::CellType::X;
			case GameState::OTurn: return GameBoard::CellType::O;
			default: std::unreachable();
			}
		});

		while (true) {
			std::print("> Player {}, enter a coordinate: ", GameBoard::cell_type_to_char(cell_type));
			std::cin >> user_input;
			std::println();

			if (user_input[0] == 'q' || user_input[0] == 'e') {
				std::println("Quitting session... Thanks for playing!");
				return 0;
			}

			auto place_success = board.place(cell_type, user_input);
			if (!place_success.has_value()) {
				std::println("[ERROR] {}", place_success.error());
				std::println("Coordinates must follow the format [A-C][1-3] where the letters represent the rows and the numbers represent the columns, and cannot be placed in an occupied cell.");
				std::println();
				continue;
			}
			break;
		}

		auto maybe_win_result = board.check_for_win();
		if (maybe_win_result.has_value()) {
			state = *maybe_win_result;
			break;
		}

		switch (state) {
		case GameState::XTurn:
			state = GameState::OTurn;
			break;
		case GameState::OTurn:
			state = GameState::XTurn;
			break;
		default: std::unreachable();
		}
	}

	board.draw();
	std::println();

	switch (state) {
	case GameState::XWin:
		std::println("Player X wins!");
		break;
	case GameState::OWin:
		std::println("Player O wins!");
		break;
	case GameState::Draw:
		std::println("It's a draw! Nobody wins!");
		break;
	default: std::unreachable();
	}
}
