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

#define loop while(true)

enum class GameState : u8 {
	Setup,
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

	void place(const CellType cell_type, const sz index)
	{
		board[index] = cell_type;
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

	[[nodiscard]] constexpr auto get_free_cells() const -> std::vector<sz>
	{
		std::vector<sz> result;
		for (sz i = 0; i < board.size(); i++) {
			if (board[i] == CellType::Empty) {
				result.push_back(i);
			}
		}
		return result;
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

enum class PlayMode : u8 {
	Undecided,
	Player,
	Computer,
};

GameState state = GameState::Setup;
GameBoard board;
PlayMode mode = PlayMode::Undecided;

std::string user_input;

auto handle_player_turn() -> bool
{
	auto cell_type = std::invoke([] -> GameBoard::CellType {
		switch (state) {
		case GameState::XTurn: return GameBoard::CellType::X;
		case GameState::OTurn: return GameBoard::CellType::O;
		default: std::unreachable();
		}
	});

	loop {
		std::print("> Player {}, enter a coordinate: ", GameBoard::cell_type_to_char(cell_type));
		std::cin >> user_input;
		std::println();

		if (user_input[0] == 'q' || user_input[0] == 'e') {
			return true;
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

	return false;
}

void handle_computer_turn()
{
	const std::vector<sz> possible_choices = std::move(board.get_free_cells());

	static std::mt19937 rng{std::random_device{}()};
	std::uniform_int_distribution<sz> dist{0, possible_choices.size() - 1};

	std::println("Computer is choosing...");
	std::println();

	const sz choice = possible_choices[dist(rng)];
	board.place(GameBoard::CellType::O, choice);
}

auto main() -> int
{
	std::println("--- CLI Tic Tac Toe ---");
	std::println("Get 3 of your symbol in a row, column, or diagonal to win!");
	std::println("Input \"quit\" at any time to quit early.");
	std::println();

	while (mode == PlayMode::Undecided) {
		std::print("> Play against another player or the computer? (p/c): ");
		std::cin >> user_input;
		std::println();

		switch (user_input[0]) {
		case 'p':
			mode = PlayMode::Player;
			state = GameState::XTurn;
			break;
		case 'c':
			mode = PlayMode::Computer;
			break;
		default:
			std::println("Invalid input. Enter 'p' for player or 'c' for computer.");
			std::println();
		}
	}

	if (mode == PlayMode::Computer) {
		while (state == GameState::Setup) {
			std::print("> You are Player X. Who plays first? (X/O): ");
			std::cin >> user_input;
			std::println();

			switch (user_input[0]) {
			case 'X':
				state = GameState::XTurn;
				break;
			case 'O':
				state = GameState::OTurn;
				break;
			default:
				std::println("Invalid input. Enter 'X' to play first or 'O' to go second.");
				std::println();
			}
		}
	}

	loop {
		board.draw();
		std::println();

		if (mode == PlayMode::Player || state == GameState::XTurn) {
			bool quit_session = handle_player_turn();
			if (quit_session) {
				std::println("Quitting session... Thanks for playing!");
				return 0;
			}
		} else {
			handle_computer_turn();
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
