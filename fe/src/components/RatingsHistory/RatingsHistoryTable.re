open EloTypes;
open ReasonReact;
open BsReactstrap;

[@bs.scope "Math"] [@bs.val] external round: float => float = "round";

let component = statelessComponent("RatingsHistoryTable");

let tableRow = (ratingsHistoryLength, i, ratingHistory) => {
  let {ratingDiff, userRating, oponentName, oponentRating, date} = ratingHistory;
  let lp = ratingsHistoryLength - i;
  let classe = ratingDiff > 0 ? "win" : "loose";
  <tr key={string_of_int(lp)} className=classe>
    <td className="lpTd"> {lp |> string_of_int |> string} </td>
    <td className="ratingDiffTd"> {ratingDiff |> string_of_int |> string} </td>
    <td className="userRatingTd"> {userRating |> string_of_int |> string} </td>
    <td className="oponentNameTd">
      {oponentName |> string}
      <span className="oponentRatingTd">
        {oponentRating |> string_of_int |> string}
      </span>
    </td>
    <td className="dateTd"> {date |> string} </td>
  </tr>;
};

let calcWins = ratingsHistory =>
  List.filter(ratingHistory => ratingHistory.ratingDiff >= 0, ratingsHistory)
  |> List.length;

let calcLooses = ratingsHistory =>
  List.filter(ratingHistory => ratingHistory.ratingDiff < 0, ratingsHistory)
  |> List.length;

let calcRatio = ratingsHistory => {
  let wins = calcWins(ratingsHistory) |> float_of_int;
  let looses = calcLooses(ratingsHistory) |> float_of_int;

  round(wins /. looses *. 100.0) /. 100.0;
};

let make = (~ratingsHistory, _children) => {
  ...component,
  render: _self => {
    let ratingsHistoryLength = List.length(ratingsHistory);
    let ratingsHistoryTrs =
      ratingsHistory |> List.mapi(tableRow(ratingsHistoryLength));

    <div className="ratingsHistoryTable">
      <RatingsHistoryChart ratingsHistory />
      <div className="topBar">
        <span className="winsLabel"> {"Wins: " |> ReasonReact.string} </span>
        <span className="winsSum">
          {calcWins(ratingsHistory) |> string_of_int |> ReasonReact.string}
        </span>
        <span className="loosesLabel">
          {"Looses: " |> ReasonReact.string}
        </span>
        <span className="loosesSum">
          {calcLooses(ratingsHistory) |> string_of_int |> ReasonReact.string}
        </span>
        <span className="loosesLabel">
          {"W/L Ratio: " |> ReasonReact.string}
        </span>
        <span className="wl_rating">
          {
            calcRatio(ratingsHistory)
            |> Js.Float.toString
            |> ReasonReact.string
          }
        </span>
      </div>
      <Table size="sm">
        <thead>
          <tr>
            <th> {string("Lp")} </th>
            <th> {string("Diff")} </th>
            <th> {string("Rating")} </th>
            <th> {string("Opponent")} </th>
            <th> {string("Date")} </th>
          </tr>
        </thead>
        <tbody> {ratingsHistoryTrs |> Array.of_list |> array} </tbody>
      </Table>
    </div>;
  },
};